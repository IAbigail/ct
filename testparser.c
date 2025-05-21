struct Pt
{
	int x;
	int y;
};

// Variable of struct type
struct Pt point;

double max(double a, double b)
{
	if (a > b)
		return a;
	else
		return b;
}

int len(char s[])
{
	int i;
	i = 0;
	while (s[i])
		i = i + 1;
	return i;
}

void main()
{
	int i;
	i = 10;
	while (i != 0)
	{
		_puti(i);
		i = i / 2;
	}
}
