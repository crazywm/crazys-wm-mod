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

#ifndef WM_FFMPEG_H
#define WM_FFMPEG_H

/// This file defines some lightweight RAII wrappers around ffmpeg classes.

#include <memory>
#include <libavutil/pixfmt.h>

extern "C" {
    struct AVPacket;
    struct AVFrame;
    struct AVFormatContext;
    struct AVDictionary;
    struct AVCodecContext;
    struct AVCodec;
    struct AVCodecParameters;
};

namespace ffmpeg {
    // Packet
    struct sFFMpegDel {
        void operator()(AVPacket*) noexcept;
        void operator()(AVFormatContext*) noexcept;
        void operator()(AVFrame*) noexcept;
        void operator()(AVCodecContext*) noexcept;
    };

    using Packet = std::unique_ptr<AVPacket, sFFMpegDel>;
    Packet alloc_packet();

    // Format Context
    using FormatContext = std::unique_ptr<AVFormatContext, sFFMpegDel>;
    FormatContext open_format(const char* file_name);
    void find_stream_info(FormatContext&, AVDictionary **options = nullptr);

    // Frame
    using Frame = std::unique_ptr<AVFrame, sFFMpegDel>;
    Frame alloc_frame();
    void unref(Frame& frame);

    // Codec Context
    using CodecContext = std::unique_ptr<AVCodecContext, sFFMpegDel>;
    CodecContext alloc_context(AVCodec* decoder);

    void load_parameters(AVCodecContext* ctx, AVCodecParameters* params);
    inline void load_parameters(CodecContext& ctx, AVCodecParameters* params) { load_parameters(ctx.get(), params); }

    /// If this returns false, than EAGAIN was returned by ffmpeg and we first must process some output
    /// before new input is accepted.
    bool send_packet(AVCodecContext* ctx, const AVPacket* pkt);
    inline bool send_packet(CodecContext& ctx, const AVPacket* pkt) { return send_packet(ctx.get(), pkt); }

    /// If this returns false, than EAGAIN or EOF was returned by ffmpeg and we first must send a new packet
    /// before new output is produced.
    bool receive_frame(AVCodecContext* ctx, AVFrame* frame);
    inline bool receive_frame(CodecContext& ctx, Frame& frame) { return receive_frame(ctx.get(), frame.get()); }

    /// A decoding loop. Sends in a packet, and decodes as many frames as possible.
    template<class F>
    void decode_packet(CodecContext& ctx, Frame& frame, const AVPacket* pkt, F&& callback) {
        bool sent = send_packet(ctx, pkt);
        while(receive_frame(ctx, frame)) {
            callback(frame);
            unref(frame);
        }

        // if we could not process data last time, then we are guaranteed to be able to do so now
        if(!sent) decode_packet(ctx, frame, pkt, std::forward<F>(callback));
    }

    void open(AVCodecContext* ctx, const AVCodec* codec, AVDictionary** options = nullptr);
    inline void open(CodecContext& ctx, const AVCodec* codec, AVDictionary** options = nullptr) {
        open(ctx.get(), codec, options);
    }

    // Image Buffer
    class sImageBuffer {
    public:
        sImageBuffer(int w, int h, AVPixelFormat format);
        ~sImageBuffer() noexcept;

        sImageBuffer(sImageBuffer&) = delete;

        uint8_t* Data[4] = {nullptr, nullptr, nullptr, nullptr};
        int Linesize[4];
    };
}

#endif //WM_FFMPEG_H
