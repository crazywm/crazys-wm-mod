/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
 * who meet on http://pinkpetal.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ffmpeg.h"
#include "CLog.h"
#include <stdexcept>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

void ffmpeg::sFFMpegDel::operator()(AVFormatContext* ctx) noexcept {
    avformat_close_input(&ctx);
}

void ffmpeg::sFFMpegDel::operator()(AVPacket* packet) noexcept {
    av_packet_free(&packet);
}

void ffmpeg::sFFMpegDel::operator()(AVFrame* frame) noexcept {
    av_frame_free(&frame);
}

void ffmpeg::sFFMpegDel::operator()(AVCodecContext* ctx) noexcept {
    avcodec_free_context(&ctx);
}

void ffmpeg::find_stream_info(ffmpeg::FormatContext& ctx, AVDictionary **options) {
    if (avformat_find_stream_info(ctx.get(), options) < 0) {
        throw std::runtime_error("Could not find stream information");
    }
}

ffmpeg::FormatContext ffmpeg::open_format(const char* file_name) {
    // open input file, and allocate format context
    AVFormatContext* fmt_ctx = nullptr;
    // open file and allocate context
    if (avformat_open_input(&fmt_ctx, file_name, nullptr, nullptr) < 0) {
        throw std::runtime_error(std::string("Could not open source file: ") + file_name);
    }
    return FormatContext{fmt_ctx};
}

ffmpeg::Packet ffmpeg::alloc_packet() {
    auto packet = av_packet_alloc();
    if(!packet)
        throw std::runtime_error("Failed to allocate packet for ffmpeg");
    return Packet{packet};
}

ffmpeg::Frame ffmpeg::alloc_frame() {
    auto frame = av_frame_alloc();
    if (!frame) {
        throw std::runtime_error("Could not allocate frame");
    }
    return Frame{frame};
}

ffmpeg::CodecContext ffmpeg::alloc_context(AVCodec* decoder) {
    AVCodecContext* dec_ctx = avcodec_alloc_context3(decoder);
    if (!dec_ctx) {
        if(decoder) {
            throw std::runtime_error(std::string("Failed to allocate the codec context for decoder ") + decoder->name);
        } else {
            throw std::runtime_error("Failed to allocate the codec context");
        }
    }
    g_LogFile.info("ffmpeg", "Allocating context for decoder: ", decoder ? decoder->long_name : "<null>");
    return CodecContext{dec_ctx};
}

void ffmpeg::load_parameters(AVCodecContext* ctx, AVCodecParameters* params) {
    int ret = avcodec_parameters_to_context(ctx, params);
    if (ret < 0) {
        throw std::runtime_error("Failed to copy video codec parameters to decoder context\n");
    }
}

bool ffmpeg::send_packet(AVCodecContext* ctx, const AVPacket* pkt) {
    int ret = avcodec_send_packet(ctx, pkt);
    // not really an error, but a status
    if(ret == AVERROR(EAGAIN)) {
        return false;
    // these are logical errors due to program problems -- turn them into std::logic_error
    } else if (ret == AVERROR_EOF) {
        g_LogFile.error("ffmpeg", "The decoder has been flushed, and no new packets can be sent to it.");
        throw std::logic_error("The decoder has been flushed, and no new packets can be sent to it.");
    } else if(ret == AVERROR(EINVAL)) {
        g_LogFile.error("ffmpeg", "Codec not opened, it is an encoder, or requires flush.");
        throw std::logic_error("Codec not opened, it is an encoder, or requires flush.");
    }

    // these are all the other errors: out of memory conditions and legitimate decoding errors
    if (ret < 0) {
        char error[1024];
        av_make_error_string(error, sizeof(error), ret);
        g_LogFile.error("ffmpeg", "Error submitting a packet for decoding: ", error);
        throw std::runtime_error(std::string("Error submitting a packet for decoding") + error);
    }
    return true;
}

bool ffmpeg::receive_frame(AVCodecContext* ctx, AVFrame* frame) {
    int ret = avcodec_receive_frame(ctx, frame);
    if (ret < 0) {
        // those two return values are special and mean there is no output
        // frame available, but there were no errors during decoding
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
            return false;

        char error[1024];
        av_make_error_string(error, sizeof(error), ret);
        g_LogFile.error("ffmpeg", "Error during decoding: ", error);
        throw std::runtime_error(std::string("Error during decoding: ") + error);
    }
    return true;
}

void ffmpeg::open(AVCodecContext* ctx, const AVCodec* codec, AVDictionary** options) {
    if(!ctx)
        throw std::logic_error("context must not be nullptr");

    if (avcodec_open2(ctx, codec, options) < 0) {
        g_LogFile.error("ffmpeg", "Failed to open codec ", codec ? codec->long_name: "<null>");
        throw std::runtime_error("Failed to open codec");
    }

    /// TODO this message is wrong for audio codecs
    g_LogFile.info("ffmpeg", "Opened video stream with pixel format ",
                   av_get_pix_fmt_name(ctx->pix_fmt));
}

void ffmpeg::unref(ffmpeg::Frame& frame) {
    av_frame_unref(frame.get());
}


ffmpeg::sImageBuffer::sImageBuffer(int w, int h, AVPixelFormat format) {
    if(av_image_alloc(Data, Linesize, w, h, format, 1) < 0) {
        throw std::runtime_error("Could not allocate image");
    }
}

ffmpeg::sImageBuffer::~sImageBuffer() noexcept{
    av_freep(&Data[0]);
}
