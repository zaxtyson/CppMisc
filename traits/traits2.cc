#include <iostream>
using std::cout;
using std::endl;

template <typename T>
void test_type(T type)
{
    if (std::is_same<T, int>::value)
        cout << "type is: int" << endl;

    if (std::is_same<T, int *>::value)
        cout << "type is: int*" << endl;

    if (std::is_same<T, int(*)[4]>::value)
        cout << "type is: int(*)[4]" << endl;

    if (std::is_same<T, int(*)[2][4]>::value)
        cout << "type is: int(*)[2][4]" << endl;
}

void test_bin_array(int array[][3], int rows)
{
    int cols = sizeof(*array) / sizeof(int);
    cout << "array cols: " << cols << endl;
}

int main(int argc, char const *argv[])
{
    int array[2][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}};
    test_type(array);
    test_type(&array);
    test_type(array + 1);
    test_type(array[1]);

    int(*ptr1)[4] = array;
    int(*ptr2)[2][4] = &array;
    cout << array[1][0] << endl;
    cout << ptr1[1][1] << endl;
    cout << ptr2[1][2] << endl;
    cout << (*ptr2)[1][2] << endl;
    cout << *(array[1] + 1) << endl;

    int a[3][3]={
    {1,2,3},
    {4,5,6},
    {7,8,9}
    };
    test_bin_array(a,3);
    cout << sizeof a << endl;

    return 0;
}
