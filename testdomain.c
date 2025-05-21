

struct Point
{
    int x;
    int y;
    double distance;
};

struct Rectangle
{
    struct Point topLeft;
    struct Point bottomRight;
    double area;
};

int globalVar;
double pi = 3.14159;
char message[100];
struct Point origin;

struct Point points[10];

int add(int a, int b)
{
    int result;
    result = a + b;
    return result;
}

double calculateDistance(struct Point p)
{
    double dx = p.x - origin.x;
    double dy = p.y - origin.y;
    return dx * dx + dy * dy;
}

void printMessage(char msg[])
{
    int i;
    i = 0;
    while (msg[i] != 0)
    {
        i = i + 1;
    }
    return;
}

struct Point createPoint(int x, int y)
{
    struct Point p;
    p.x = x;
    p.y = y;
    p.distance = 0.0;
    return p;
}

int main()
{
    int x;
    int y;
    int x;
    struct Point p;
    struct Rectangle rect;

    x = 10;
    y = 20;
    p.x = x;
    p.y = y;

    int sum = add(x, y);
    p.distance = calculateDistance(p);

    int numbers[5];
    int i;
    i = 0;
    while (i < 5)
    {
        numbers[i] = i * i;
        i = i + 1;
    }

    {
        int x;
        x = 5;

        {
            double z = 3.14;
            z = z * x;
        }
    }

    rect.topLeft = createPoint(0, 0);
    rect.bottomRight = createPoint(100, 100);
    rect.area = (rect.bottomRight.x - rect.topLeft.x) *
                (rect.bottomRight.y - rect.topLeft.y);

    return 0;
}