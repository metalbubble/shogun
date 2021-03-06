#include <shogun/base/init.h>
#include <shogun/io/SGIO.h>
#include <shogun/lib/SGMatrix.h>
#include <shogun/io/SerializableAsciiFile.h>
#include <shogun/features/SparseFeatures.h>

using namespace shogun;
int main(int argc, char** argv)
{
	init_shogun_with_defaults();
	sg_io->set_loglevel(MSG_DEBUG);

	/* create feature data matrix */
	SGMatrix<int32_t> data(3, 20);

	/* fill matrix with random data */
	for (index_t i=0; i<10*3; ++i)
	{
		if (i%2==0)
			data.matrix[i]=0;
		else
			data.matrix[i]=CMath::random(1, 9);
	}

	/* create sparse features */
	CSparseFeatures<int32_t>* sparse_features=new CSparseFeatures<int32_t>(data);

	CSerializableAsciiFile* file;

	file=new CSerializableAsciiFile("sparseFeatures.txt", 'w');
	sparse_features->save_serializable(file);
	file->close();
	SG_UNREF(file);

	/* this will fail with a warning, same with CSerializableHdf5File and xml serialization*/
	CSparseFeatures<int32_t>* sparse_features_loaded = new CSparseFeatures<int32_t>();
	file = new CSerializableAsciiFile("sparseFeatures.txt", 'r');
	sparse_features_loaded->load_serializable(file);
	SG_UNREF(file);
}
