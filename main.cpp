#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>
#define NaN (float)sqrt(-1.0)
#define CLOCK_PER_MILESECOND 1000
#define uint unsigned int
using namespace std;

template<typename T>
void printArr(T *arr, int len) {
    for (int i=0; i<len; ++i) {
        if (i > 100) {
            cout << "......";
            break;
        }
        if (arr[i] != arr[i]) cout << setw(6) << setiosflags(ios::left) <<  "NaN";
        else cout << setw(6) << setprecision(3) << setiosflags(ios::left) << arr[i];
    }
    cout << endl;
}

/**
 * 分段双调排序
 * @param data      需要分段排序的n个float值
 * @param seg_id    data中n个元素各自所在的段编号
 * @param seg_start 共有m+1个元素，前m个分别给 出0..m-1共m个段的起始位置，seg_start[m]保证等于n
 * @param n         data的长度
 * @param m         段的个数
 */
void segmentedBitonicSort(float* data, int* seg_id, int* seg_start, int n, int m) {
    float *arr = data;
    for (int i=0; i<m; i++) {
        // 当前段长度
        int len = seg_start[i+1] - seg_start[i];
        {
            // 取不小于len的最小2的n次幂
            uint mLen = 1;
            while (mLen < len) mLen <<= 1u;
            for (uint step = 1; step < len; step <<= 1u) {
                // 前半子序列降序后半升序，最后一次merge时升序，构建若干长度为
                bool asc = (step == mLen>>1u);
                for (uint start = 0; start < len; start += step << 1u) {
                    // n不为2的n次幂时，下标不可超限，即认为超限的元素为最大值所以不做swap
                    for (uint cur = start; cur < start + step && cur < len - step; cur++) {
                        // 把NaN视为最大值排到最后
                        float tmp_a = arr[cur] == arr[cur] ? arr[cur] : MAXFLOAT;
                        float tmp_b = arr[cur + step] == arr[cur + step] ? arr[cur + step] : MAXFLOAT;
                        if (!((tmp_a > tmp_b) ^ asc)) {
                            float tmp = arr[cur];
                            arr[cur] = arr[cur + step];
                            arr[cur + step] = tmp;
                        }
                    }
                    // 对每一个子序列内部排序
                    for (uint s = step >> 1u; s > 0; s >>= 1u) {
                        // 当前子序列的最大下标，遍历时不可超出这个范围
                        uint curLen = start + (step << 1u) < len ? start + (step << 1u) : len;
                        for (uint left = start; left < curLen; left += s << 1u) {
                            for (uint cur = left; cur < left + s && cur < curLen - s; cur++) {
                                // 把NaN视为最大值排到最后
                                float tmp_a = arr[cur] == arr[cur] ? arr[cur] : MAXFLOAT;
                                float tmp_b = arr[cur + s] == arr[cur + s] ? arr[cur + s] : MAXFLOAT;
                                if (!((tmp_a > tmp_b) ^ asc)) {
                                    float tmp = arr[cur];
                                    arr[cur] = arr[cur + s];
                                    arr[cur + s] = tmp;
                                }
                            }
                        }
                    }
                    asc = !asc;
                }
            }
        }
        arr += len;
    }
}

void test_sort(float* data, int* seg_id, int* seg_start, int n, int m, int id) {
    cout << "=============SAMPLE " << id << " BEGIN=============" << endl;
    cout << "data:   ";
    printArr(data, n);
    cout << "seg_id: ";
    printArr(seg_id, n);
    cout << "seg_start: ";
    printArr(seg_start, m+1);
    cout << "n: " << n << endl;
    cout << "m: " << m << endl;
    clock_t startTime, endTime;
    startTime = clock();
    segmentedBitonicSort(data, seg_id, seg_start, n, m);
    endTime = clock();
    cout << "result: ";
    printArr(data, n);
    cout << (double)(endTime - startTime) / CLOCK_PER_MILESECOND << "ms" << endl;
    cout << "=============SAMPLE " << id << " END=============" << endl << endl;
}

/**
 * 给定的样例
 */
void test_case1() {
    float data[5]={0.8, 0.2, 0.4, 0.6, 0.5};
    int seg_id[5]={0, 0, 1, 1, 1};
    int seg_start[3]={0,2,5};
    int n=5;
    int m=2;
    test_sort(data, seg_id, seg_start, n, m, 1);
}

/**
 * 1000个段，每段长度为4~24随机
 */
void test_case2() {
    int m = 1000, n = 0;
    int *seg_start = new int[m + 1];
    seg_start[0] = 0;
    for (int i=0; i<m; i++) {
        n += ((int)(random() % 20) + 4);
        seg_start[i + 1] = n;
    }
    auto *data = new float[n];
    auto *seg_id = new int[n];
    int idx = 0;
    for (int i=0; i<m; i++) {
        int len = seg_start[i + 1] - seg_start[i];
        for (int j=0; j<len; j++) {
            seg_id[idx] = i;
            data[idx++] = random() % 4 == 0 ? NaN : (float) (random() % 1000) / 1000;
        }
    }
    test_sort(data, seg_id, seg_start, n, m, 2);
}

int main () {
    test_case1();
    test_case2();
    return 0;
}