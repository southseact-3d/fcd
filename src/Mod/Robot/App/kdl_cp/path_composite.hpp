#ifndef KDL_PATHCOMPOSITE_H
#define KDL_PATHCOMPOSITE_H

#include "frames.hpp"
#include "frames_io.hpp"
#include "path.hpp"
#include <vector>

namespace KDL {

	 /**
	  * A Path being the composition of other Path objects.
	  *
	  * For several of its methods, this class needs to lookup the segment corresponding to a value
	  * of the path variable s.  To increase efficiency, this value is cached.
	  *
	  * \TODO Currently a linear search is used to look up the segment. A binary search is more efficient.  Can STL be used for this ?
	  * \TODO Increase the efficiency for caching for the common case of a fine grained monotonously increasing path variable s.
	  *
	  * \TODO For all Path.., VelocityProfile.., Trajectory... check the bounds on the inputs with asserts.
	  *
	  * \TODO explain this routine in the wiki.
	  *
	  * @ingroup Motion
	  */
	 class Path_Composite : public Path
	{
		typedef std::vector< std::pair<Path*,bool> > PathVector;
		typedef std::vector<double>    DoubleVector;

		PathVector gv;
		DoubleVector   dv;
		double pathlength;

		// lookup mechanism :
		mutable double cached_starts;
		mutable double cached_ends;
		mutable int    cached_index;
		double Lookup(double s) const;
	public:


		Path_Composite();

		/**
		 * Adds a Path* to this composite
		 */
		void Add(Path* geom, bool aggregate=true);


		virtual double LengthToS(double length);
		/**
		 * Returns the total path length of the trajectory
		 * (has dimension LENGTH)
		 * This is not always a physical length , ie when dealing with rotations
		 * that are dominant.
		 */
		virtual double PathLength();

		/**
		 * Returns the Frame at the current path length s
		 */
		virtual Frame Pos(double s) const;

		/**
		 * Returns the velocity twist at path length s theta and with
		 * derivative of s == sd
		 */
		virtual Twist Vel(double s,double sd) const;

		/**
		 * Returns the acceleration twist at path length s and with
		 * derivative of s == sd, and 2nd derivative of s == sdd
		 */
		virtual Twist Acc(double s,double sd,double sdd) const;

		virtual Path* Clone();

		/**
		 * Writes one of the derived objects to the stream
		 */
		virtual void Write(std::ostream& os);

		/**
		 * returns the number of underlying segments.
		 */
		virtual int GetNrOfSegments();

		/**
		 * returns a pointer to the underlying Path of the given segment number i.
		 * \param i segment number
		 * \return pointer to the underlying Path
		 * \warning The pointer is still owned by this class and is lifetime depends on the lifetime
		 *          of this class.
		 */
		virtual Path* GetSegment(int i);

		/**
		 * gets the length to the end of the given segment.
		 * \param i segment number
		 * \return length to the end of the segment, i.e. the value for s corresponding to the end of
		 *         this segment.
		 */
		virtual double GetLengthToEndOfSegment(int i);

		/**
		 * \param s [INPUT] path length variable for the composite.
		 * \param segment_number [OUTPUT] segments that corresponds to the path length variable s.
		 * \param inner_s [OUTPUT] path length to use within the segment.
		 */
		virtual void GetCurrentSegmentLocation(double s, int &segment_number, double& inner_s);

		/**
		 * gets an identifier indicating the type of this Path object
		 */
		virtual IdentifierType getIdentifier() const {
			return ID_COMPOSITE;
		}

		virtual ~Path_Composite();
	};



}


#endif
