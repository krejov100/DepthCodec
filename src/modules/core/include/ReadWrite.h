//
// Created by philip on 8/24/18.
//

#ifndef DEPTHCODEC_CVHELPERS_H
#define DEPTHCODEC_CVHELPERS_H

#include <opencv2/opencv.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>

/// This blog post also discusses compression of streams which could be great to do but not here!
///https://cheind.wordpress.com/2011/12/06/serialization-of-cvmat-objects-using-boost/

BOOST_SERIALIZATION_SPLIT_FREE(::cv::Mat)

namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive & ar, cv::Size& size, const unsigned int version)
        {
            ar & size.width;
            ar & size.height;
        }

        /// Save serialization support for cv::Mat
        template<class Archive>
        void save(Archive & ar, const ::cv::Mat& m, const unsigned int version)
        {
            size_t elem_size = m.elemSize();
            size_t elem_type = m.type();

            ar & m.cols;
            ar & m.rows;
            ar & elem_size;
            ar & elem_type;

            const size_t data_size = m.cols * m.rows * elem_size;
            ar & make_array(m.ptr(), data_size);
        }

        /// Load serialization support for cv::Mat
        template<class Archive>
        void load(Archive & ar, ::cv::Mat& m, const unsigned int version)
        {
            int cols, rows;
            size_t elem_size, elem_type;

            ar & cols;
            ar & rows;
            ar & elem_size;
            ar & elem_type;

            m.create(rows, cols, elem_type);

            size_t data_size = m.cols * m.rows * elem_size;
            ar & make_array(m.ptr(), data_size);
        }

    }
}
#endif //DEPTHCODEC_CVHELPERS_H
