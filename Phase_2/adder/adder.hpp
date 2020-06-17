class Adder {
	unsigned int offset;
public:
	Adder(bool with_carry);
	void add(unsigned int a, unsigned int b, unsigned int* result);
};
