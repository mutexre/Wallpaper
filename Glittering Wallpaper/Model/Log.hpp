//
//  Log.h
//  Wallpaper
//
//  Created by mutexre on 12/09/16.
//
//

template <typename T, size_t Size = 50>
struct Log
{
private:
    array<T, Size> data;
    size_t head = 0;
    size_t itemsCount = 0;
    T sum = 0;
    
public:
    Log(T initialValue) {
        for (int i = 0; i < Size; i++)
            data[i] = initialValue;
    }
    
    void add(T value)
    {
        head = (head + 1) % Size;
        
        T oldValue = data[head];
        sum += value - oldValue;
        data[head] = value;
        
        if (itemsCount < Size)
            itemsCount++;
    }
    
    T operator[](size_t i) const {
        return get(i);
    }
    
    T get(off_t i) const
    {
        off_t offset = head - i;
        if (offset < 0)
            offset += Size;
        
        return data[offset];
    }
    
    void set(size_t i, T x)
    {
        off_t offset = head - i;
        if (offset < 0)
            offset += Size;
        
        data[offset] = x;
    }
    
    T getSum() const {
        return sum;
    }
    
    T getMeanValue() const {
        return itemsCount > 0 ? sum / itemsCount : 0;
    }
};
