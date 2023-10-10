// MyTest4FC.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

int Func_InstCall(int (*func)(int, int), int x, int y);
int Inst_Add(int a, int b);
int Inst_Sup(int a, int b);
int Inst_Max(int a, int b);
int Inst_Min(int a, int b);

extern int SocketServer(char* buff);

int main()
{
    int a = 5;
    int b = 7;
    int c = 0;
    int (*func)(int, int) = NULL;

    //func = Inst_Add;
    c = Func_InstCall(Inst_Add, a, b);

    std::cout << "默认回调函数\n";
    std::cout << "func result: " << c << "\n" << std::endl;

    std::cout << "Server\n";

    char inst[512] = { 0 };
    int ret = SocketServer(inst);

    if (!strcmp(inst, "ADD"))
    {
        func = Inst_Add;
    }
    else if (!strcmp(inst, "SUP"))
    {
        func = Inst_Sup;
    }
    else if (!strcmp(inst, "MAX"))
    {
        func = Inst_Max;
    }
    else if (!strcmp(inst, "MIN"))
    {
        func = Inst_Min;
    }
    else
    {
        ret = -1;
    }
    c = Func_InstCall(func, a, b);

    std::cout << "func result: " << c << "\n" << std::endl;
    std::cout << ret << std::endl;

    std::cin >> ret;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

int Func_InstCall(int (*func)(int, int), int x, int y)
{
    return func(x, y);
}

int Inst_Add(int a, int b)
{
    return (a + b);
}

int Inst_Sup(int a, int b)
{
    return (a - b);
}

int Inst_Max(int a, int b)
{
    return ((a >= b ? a : b));
}

int Inst_Min(int a, int b)
{
    return ((a <= b ? a : b));
}


