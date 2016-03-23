/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */

#include <cstring>
#include "core/opendavinci.h"

#include "core/base/Hash.h"
#include "core/base/Deserializer.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Serializer.h"


//#include "generated/cartesian/Point2.h"
#include "automotivedata/Point2.h"

namespace cartesian {
		using namespace std;
		using namespace core::base;
	
	
		Point2::Point2() :
		    SerializableData(), Visitable()
			, m_p()
		{
			m_p = new float[getSize_P()];
			for(uint32_t i = 0; i < getSize_P(); i++) {
				m_p[i] = 0;
			}
		}
	
		Point2::Point2(
			const float *val0
		) :
		    SerializableData(), Visitable()
			, m_p() // The content from obj will be copied in the copy-constructor's body.
		{
			m_p = new float[getSize_P()];
			::memcpy(m_p, val0, getSize_P() * sizeof(*m_p));
		}
	
		Point2::Point2(const Point2 &obj) :
		    SerializableData(), Visitable()
			, m_p() // The content from obj will be copied in the copy-constructor's body.
		{
			m_p = new float[getSize_P()];
			::memcpy(m_p, obj.m_p, obj.getSize_P() * sizeof(*m_p));
		}
		
		Point2::~Point2() {
			OPENDAVINCI_CORE_DELETE_ARRAY(m_p);
		}
	
		Point2& Point2::operator=(const Point2 &obj) {
			::memcpy(m_p, obj.m_p, obj.getSize_P() * sizeof(*m_p));
			return (*this);
		}
	
		int32_t Point2::ID() {
			return 51;
		}
	
		const string Point2::ShortName() {
			return "Point2";
		}
	
		const string Point2::LongName() {
			return "cartesian.Point2";
		}
	
		float* Point2::getP() {
			return m_p;
		}
		
		uint32_t Point2::getSize_P() const {
			return 2;
		}
	
		void Point2::accept(core::base::Visitor &v) {
			(void)v; // Avoid unused parameter warning.
		}
	
		const string Point2::toString() const {
			stringstream s;
	
	
			s << "p: (";
			for(uint32_t i = 0; i < getSize_P(); i++) {
				s << (m_p[i]) << (((i+1) < getSize_P()) ? ", " : "");
			}
			s << ")" << " ";
	
			return s.str();
		}
	
		ostream& Point2::operator<<(ostream &out) const {
	
			SerializationFactory& sf = SerializationFactory::getInstance();
	
			core::SharedPointer<Serializer> s = sf.getSerializer(out);
	
			s->write(1,
					m_p, getSize_P() * (sizeof(float)/sizeof(char)));
			return out;
		}
	
		istream& Point2::operator>>(istream &in) {
	
			SerializationFactory& sf = SerializationFactory::getInstance();
	
			core::SharedPointer<Deserializer> d = sf.getDeserializer(in);
	
			d->read(1,
			       m_p, getSize_P() * (sizeof(float)/sizeof(char)));
			return in;
		}
} // cartesian
