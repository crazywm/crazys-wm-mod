#ifndef WM_HDYNAMICTRAITHANDLE_H
#define WM_HDYNAMICTRAITHANDLE_H

class ITraitsCollection;

class hDynamicTraitHandle {
public:
    hDynamicTraitHandle(ITraitsCollection* c, uint64_t h) : collection(c), handle(h) { }
    ~hDynamicTraitHandle();
    hDynamicTraitHandle(hDynamicTraitHandle&) = delete;
    hDynamicTraitHandle(hDynamicTraitHandle&&);
private:
    ITraitsCollection* collection;
    uint64_t handle;
};

#endif //WM_HDYNAMICTRAITHANDLE_H
