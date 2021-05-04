int compute ()
{
    int* x;
    int y = 10;

    x = &y;
    *x += 1;
    x += 2;

  return y;
}
