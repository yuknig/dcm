#ifndef _STREAM_IMPL_1C77CE8C_0500_4A79_AF0F_50A7EAA171BF_
#define _STREAM_IMPL_1C77CE8C_0500_4A79_AF0F_50A7EAA171BF_

// template wrapper that implements StreamRead::Impl interface
// by forwarding call to functions (non virtual) of ImplT
template <typename InterfaceT, typename ImplT>
class StreamReadImplT : public InterfaceT, protected ImplT
{
public:
    // constructor forwarding arguments to ImplT
    template <typename... ArgsT>
    StreamReadImplT(ArgsT&&... args)
        : ImplT(std::forward<ArgsT>(args)...)
    {}

    // InterfaceT implementation
    size_t read(void* a_dest, size_t a_size_in_bytes) override
    {
        return ImplT::read(a_dest, a_size_in_bytes);
    }

    size_t size() const override
    {
        return ImplT::size();
    }

    size_t pos() const override
    {
        return ImplT::pos();
    }

    bool seek(size_t a_pos) override
    {
        return ImplT::seek(a_pos);
    }

    bool advance(const ptrdiff_t a_offset) override
    {
        return ImplT::advance(a_offset);
    }
};

#endif
