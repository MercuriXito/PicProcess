#ifndef DISCRIPTOREXAMPLE_H
#define DISCRIPTOREXAMPLE_H

// 傅里叶描述子的一个例子
// 因为傅里叶描述子是针对边界的点的，实验也是针对边界点的不同项数的重建，
// 所以这里就只有一个例子，并不是目标图像的某种变换

#include <vector>

class QImage;

class DiscriptorExample
{
public:
    DiscriptorExample();
    DiscriptorExample(std::vector<std::pair<int, int> > );

    int width, height;

    int getNumItem() const;

    // 项数重构，输入点的坐标构成复数，进行FFT，然后进行截断进行IFFT，
    void refactorItem(int m);

    // 展示重构前和重构后的边界
    QImage show(bool original);

private:
    std::vector<std::pair<int, int> > boundary;
    std::vector<std::pair<int, int> > refactored;

    void init();

};

#endif // DISCRIPTOREXAMPLE_H
