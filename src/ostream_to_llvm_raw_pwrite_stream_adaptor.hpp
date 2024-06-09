/*
I wish this class didnt need to exist, I really do. But here we are...

Wraps a std::ostream in an interface of llvm::raw_ostream and its subclass llvm::raw_fd_ostream. This gives an object that can be given to LLVM output methods.

I need this since llvm::raw_ostream objects can only be constructed from a filename, thus std::cout isn't an option.
*/
#pragma once

#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Error.h>

class OStreamToLLVMRawPWriteStreamAdaptor : public llvm::raw_pwrite_stream {
public:
    OStreamToLLVMRawPWriteStreamAdaptor (std::ostream* stream) : stream_{stream} {}

    void write_impl(const char* ptr, size_t size) override {
        stream_->write(ptr, size);
        pos_ += size;
    }

    void pwrite_impl(const char* ptr, size_t size, uint64_t offset) override {
        uint64_t pos = tell();
        seek(offset);
        write(ptr, size);
        seek(pos);
    }

    uint64_t seek(uint64_t offset) {
        //assert(SupportsSeeking && "Stream does not support seeking!");
        flush();
        #ifdef _WIN32
            pos_ = ::_lseeki64(FD, offset, SEEK_SET);
        #else
            pos_ = ::lseek(FD, offset, SEEK_SET);
        #endif

        if (pos_ == (uint64_t)-1) {
            //error_detected(errnoAsErrorCode());
        }

        return pos_;
    }

    std::uint64_t current_pos() const override { 
        return pos_; 
    }

protected:
    void error_detected(std::error_code EC) { 
        this->EC = EC; 
    }

    std::ostream* stream_;
    std::uint64_t pos_ = 0;

    int FD;
    bool ShouldClose;
    bool SupportsSeeking = false;
    bool IsRegularFile = false;
    mutable std::optional<bool> HasColors;

    std::error_code EC;

    #ifdef _WIN32
        /// True if this fd refers to a Windows console device. Mintty and other
        /// terminal emulators are TTYs, but they are not consoles.
        bool IsWindowsConsole = false;
    #endif
};
