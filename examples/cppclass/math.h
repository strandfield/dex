
/*!
 * \fn int min(int a, int b)
 * \brief returns the minimum of two integers
 */
inline int min(int a, int b)
{
  return a < b ? a : b;
}

/*!
 * \fn int max(int a, int b)
 * \brief returns the maximum of two integers
 */
inline int max(int a, int b)
{
  return a > b ? a : b;
}


/*!
 * \class Point
 * \brief provides a 2D-point with integer coordinates
 */
struct Point
{
  int x;
  int y;
};
