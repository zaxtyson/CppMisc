template <typename T>
class stupid_ptr
{
private:
    T *_ptr;

public:
    stupid_ptr(T *ptr) : _ptr(ptr) {}
    ~stupid_ptr() { delete _ptr; }
    T *operator->() const { return _ptr; }
    T &operator*() const { return *_ptr; }
    operator bool() const { return _ptr; }
    T *release()
    {
        T *ptr2 = _ptr;
        _ptr = nullptr;
        return ptr2;
    }
    T &operator=(stupid_ptr &rhs)
    {
        rhs.swap(this);
        return *this;
    }
    stupid_ptr(stupid_ptr &other)
    {
        _ptr = other.release();
    }
    void swap(stupid_ptr &other){
        std::swap(_ptr, other._ptr);
    }
};