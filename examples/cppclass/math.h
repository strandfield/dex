
/*!
 * \fn int min(int a, int b)
 * \ingroup math
 * \brief returns the minimum of two integers
 */
inline int min(int a, int b)
{
  return a < b ? a : b;
}

/*!
 * \fn int max(int a, int b)
 * \ingroup math
 * \brief returns the maximum of two integers
 */
inline int max(int a, int b)
{
  return a > b ? a : b;
}


/*!
 * \class Point
 * \brief provides a 2D-point with integer coordinates
 * 
 * \begin{since}[v0.1.0]
 * See also the following math related functions:
 * \makegrouptable math
 * \end{since}
 *
 * \begin{code}[cpp]
 *   Point p;
 *   p.x = 5;
 * \end{code}
 */
struct Point
{
  int x;
  int y;
};

/*!
 * \fn bool operator==(const Point& lhs, const Point& rhs)
 * \nonmember
 * \brief compares two points for equality
 */
inline bool operator==(const Point& lhs, const Point& rhs)
{
  return lhs.x == rhs.y && lhs.y == rhs.y;
}
