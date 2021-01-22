#include "main.h"
#include "Utils.h"

void TransformBs(ByteStream* bs, BigInt key) {
	unsigned int len = 0;
	unsigned char* data = bs->GetData(&len);
	for (int i = 0; i < len; i++) {
		data[i] ^= key.to_int();
	}
}
bool is_prime(int p) {
	for (int i = 2; i < p; i++) {
		if (p % i == 0) return false;
	}
	return true;
}

int gen_prime(int start, int end)
{
	int jump = rand() % (end - start) / 5;
	for (unsigned long long i = start + jump; i < end; i++) {
		if (is_prime(i)) return i;
	}
	return 17;
}