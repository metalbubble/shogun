#include <shogun/lib/config.h>
#include <shogun/base/init.h>
#include <shogun/lib/common.h>
#include <shogun/lib/SGMatrix.h>
#include <shogun/io/HDF5File.h>
#include <shogun/mathematics/Math.h>

using namespace shogun;

int main(int argc, char** argv)
{
	init_shogun_with_defaults();
#ifdef HAVE_HDF5
	CHDF5File* hdf = new CHDF5File((char*) "../data/australian.libsvm.h5",'r', "/data/data");
	float64_t* mat;
	int32_t num_feat;
	int32_t num_vec;
	hdf->get_matrix(mat, num_feat, num_vec);

	SGMatrix<float64_t>::display_matrix(mat, num_feat, num_vec);
#endif

	exit_shogun();
	return 0;
}

