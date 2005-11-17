#ifndef __itkIndexLexicographicCompare_h
#define __itkIndexLexicographicCompare_h

namespace itk {

namespace Functor
{
/** \class IndexLexicographicCompare
 * \brief Order Index instances lexicographically.
 *
 * This is a comparison functor suitable for storing Offset instances
 * in an STL container.  The ordering is total and unique but has
 * little geometric meaning.
 */
template<unsigned int VOffsetDimension>
class OffsetLexicographicCompare
{
public:
  bool operator()(Offset<VOffsetDimension> const& l,
                  Offset<VOffsetDimension> const& r) const
    {
    for(unsigned int i=0; i < VOffsetDimension; ++i)
      {
      if(l.m_Offset[i] < r.m_Offset[i])
        {
        return true;
        }
      else if(l.m_Offset[i] > r.m_Offset[i])
        {
        return false;
        }
      }
    return false;
    }
};
}

}

#endif
