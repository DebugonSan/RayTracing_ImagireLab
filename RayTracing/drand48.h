#ifndef DRAND48H
#define DRAND48H

#include<random>

double drand48()
{
	std::random_device rand;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rand());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_real_distribution<> rand48(0.0, 1.0);    // [1.0, 2.0] 範囲の一様乱数
	return rand48(mt);
}

#endif