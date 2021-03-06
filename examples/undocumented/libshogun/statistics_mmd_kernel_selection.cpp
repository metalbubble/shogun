/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2012 Heiko Strathmann
 */

#include <shogun/base/init.h>
#include <shogun/statistics/LinearTimeMMD.h>
#include <shogun/statistics/QuadraticTimeMMD.h>
#include <shogun/statistics/MMDKernelSelectionCombOpt.h>
#include <shogun/statistics/MMDKernelSelectionCombMaxL2.h>
#include <shogun/statistics/MMDKernelSelectionOptSingle.h>
#include <shogun/statistics/MMDKernelSelectionMax.h>
#include <shogun/features/streaming/StreamingFeatures.h>
#include <shogun/features/streaming/StreamingDenseFeatures.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/kernel/GaussianKernel.h>
#include <shogun/kernel/CombinedKernel.h>
#include <shogun/mathematics/Statistics.h>

using namespace shogun;

void test_kernel_choice_linear_time_mmd_opt_comb()
{
	index_t m=8;
	index_t d=3;
	SGMatrix<float64_t> data(d,2*m);
	for (index_t i=0; i<2*d*m; ++i)
		data.matrix[i]=i;

	/* create data matrix for each features (appended is not supported) */
	SGMatrix<float64_t> data_p(d, m);
	memcpy(&(data_p.matrix[0]), &(data.matrix[0]), sizeof(float64_t)*d*m);

	SGMatrix<float64_t> data_q(d, m);
	memcpy(&(data_q.matrix[0]), &(data.matrix[d*m]), sizeof(float64_t)*d*m);

	/* normalise data to get some reasonable values for Q matrix */
	float64_t max_p=data_p.max_single();
	float64_t max_q=data_q.max_single();

	SG_SPRINT("%f, %f\n", max_p, max_q);

	for (index_t i=0; i<d*m; ++i)
	{
		data_p.matrix[i]/=max_p;
		data_q.matrix[i]/=max_q;
	}

	data_p.display_matrix("data_p");
	data_q.display_matrix("data_q");

	CDenseFeatures<float64_t>* features_p=new CDenseFeatures<float64_t>(data_p);
	CDenseFeatures<float64_t>* features_q=new CDenseFeatures<float64_t>(data_q);

	/* create stremaing features from dense features */
	CStreamingFeatures* streaming_p=
			new CStreamingDenseFeatures<float64_t>(features_p);
	CStreamingFeatures* streaming_q=
			new CStreamingDenseFeatures<float64_t>(features_q);

	/* create kernels with sigmas 2^5 to 2^7 */
	CCombinedKernel* combined_kernel=new CCombinedKernel();
	for (index_t i=5; i<=7; ++i)
	{
		/* shoguns kernel width is different */
		float64_t sigma=CMath::pow(2, i);
		float64_t sq_sigma_twice=sigma*sigma*2;
		combined_kernel->append_kernel(new CGaussianKernel(10, sq_sigma_twice));
	}

	/* create MMD instance */
	CLinearTimeMMD* mmd=new CLinearTimeMMD(combined_kernel, streaming_p,
			streaming_q, m);

	/* kernel selection instance with regularisation term */
	CMMDKernelSelectionCombOpt* selection=new CMMDKernelSelectionCombOpt(mmd,
			10E-5);

	/* start streaming features parser */
	streaming_p->start_parser();
	streaming_q->start_parser();

	CKernel* result=selection->select_kernel();
	CCombinedKernel* casted=dynamic_cast<CCombinedKernel*>(result);
	ASSERT(casted);
	SGVector<float64_t> weights=casted->get_subkernel_weights();
	weights.display_vector("weights");

	/* assert weights against matlab */
//	w_opt =
//	   0.761798190146441
//	   0.190556117891148
//	   0.047645691962411
	ASSERT(CMath::abs(weights[0]-0.761798190146441)<10E-15);
	ASSERT(CMath::abs(weights[1]-0.190556117891148)<10E-15);
	ASSERT(CMath::abs(weights[2]-0.047645691962411)<10E-15);


	/* start streaming features parser */
	streaming_p->end_parser();
	streaming_q->end_parser();

	SG_UNREF(selection);
	SG_UNREF(result);
}

void test_kernel_choice_linear_time_mmd_maxl2_comb()
{
	index_t m=8;
	index_t d=3;
	SGMatrix<float64_t> data(d,2*m);
	for (index_t i=0; i<2*d*m; ++i)
		data.matrix[i]=i;

	/* create data matrix for each features (appended is not supported) */
	SGMatrix<float64_t> data_p(d, m);
	memcpy(&(data_p.matrix[0]), &(data.matrix[0]), sizeof(float64_t)*d*m);

	SGMatrix<float64_t> data_q(d, m);
	memcpy(&(data_q.matrix[0]), &(data.matrix[d*m]), sizeof(float64_t)*d*m);

	/* normalise data to get some reasonable values for Q matrix */
	float64_t max_p=data_p.max_single();
	float64_t max_q=data_q.max_single();

	SG_SPRINT("%f, %f\n", max_p, max_q);

	for (index_t i=0; i<d*m; ++i)
	{
		data_p.matrix[i]/=max_p;
		data_q.matrix[i]/=max_q;
	}

	data_p.display_matrix("data_p");
	data_q.display_matrix("data_q");

	CDenseFeatures<float64_t>* features_p=new CDenseFeatures<float64_t>(data_p);
	CDenseFeatures<float64_t>* features_q=new CDenseFeatures<float64_t>(data_q);

	/* create stremaing features from dense features */
	CStreamingFeatures* streaming_p=
			new CStreamingDenseFeatures<float64_t>(features_p);
	CStreamingFeatures* streaming_q=
			new CStreamingDenseFeatures<float64_t>(features_q);

	/* create kernels with sigmas 2^5 to 2^7 */
	CCombinedKernel* combined_kernel=new CCombinedKernel();
	for (index_t i=5; i<=7; ++i)
	{
		/* shoguns kernel width is different */
		float64_t sigma=CMath::pow(2, i);
		float64_t sq_sigma_twice=sigma*sigma*2;
		combined_kernel->append_kernel(new CGaussianKernel(10, sq_sigma_twice));
	}

	/* create MMD instance */
	CLinearTimeMMD* mmd=new CLinearTimeMMD(combined_kernel, streaming_p,
			streaming_q, m);

	/* kernel selection instance with regularisation term */
	CMMDKernelSelectionCombMaxL2* selection=new CMMDKernelSelectionCombMaxL2(
			mmd, 10E-5);

	/* start streaming features parser */
	streaming_p->start_parser();
	streaming_q->start_parser();

	CKernel* result=selection->select_kernel();
	CCombinedKernel* casted=dynamic_cast<CCombinedKernel*>(result);
	ASSERT(casted);
	SGVector<float64_t> weights=casted->get_subkernel_weights();
	weights.display_vector("weights");

	/* assert weights against matlab */
//	w_l2 =
//	   0.761798188424313
//	   0.190556119182660
//	   0.047645692393028
	ASSERT(CMath::abs(weights[0]-0.761798188424313)<10E-15);
	ASSERT(CMath::abs(weights[1]-0.190556119182660)<10E-15);
	ASSERT(CMath::abs(weights[2]-0.047645692393028)<10E-15);

	/* start streaming features parser */
	streaming_p->end_parser();
	streaming_q->end_parser();

	SG_UNREF(selection);
	SG_UNREF(result);
}

void test_kernel_choice_linear_time_mmd_opt_single()
{
	index_t m=8;
	index_t d=3;
	SGMatrix<float64_t> data(d,2*m);
	for (index_t i=0; i<2*d*m; ++i)
		data.matrix[i]=i;

	/* create data matrix for each features (appended is not supported) */
	SGMatrix<float64_t> data_p(d, m);
	memcpy(&(data_p.matrix[0]), &(data.matrix[0]), sizeof(float64_t)*d*m);

	SGMatrix<float64_t> data_q(d, m);
	memcpy(&(data_q.matrix[0]), &(data.matrix[d*m]), sizeof(float64_t)*d*m);

	/* normalise data to get some reasonable values for Q matrix */
	float64_t max_p=data_p.max_single();
	float64_t max_q=data_q.max_single();

	SG_SPRINT("%f, %f\n", max_p, max_q);

	for (index_t i=0; i<d*m; ++i)
	{
		data_p.matrix[i]/=max_p;
		data_q.matrix[i]/=max_q;
	}

	data_p.display_matrix("data_p");
	data_q.display_matrix("data_q");

	CDenseFeatures<float64_t>* features_p=new CDenseFeatures<float64_t>(data_p);
	CDenseFeatures<float64_t>* features_q=new CDenseFeatures<float64_t>(data_q);

	/* create stremaing features from dense features */
	CStreamingFeatures* streaming_p=
			new CStreamingDenseFeatures<float64_t>(features_p);
	CStreamingFeatures* streaming_q=
			new CStreamingDenseFeatures<float64_t>(features_q);

	/* create kernels with sigmas 2^5 to 2^7 */
	CCombinedKernel* combined_kernel=new CCombinedKernel();
	for (index_t i=5; i<=7; ++i)
	{
		/* shoguns kernel width is different */
		float64_t sigma=CMath::pow(2, i);
		float64_t sq_sigma_twice=sigma*sigma*2;
		combined_kernel->append_kernel(new CGaussianKernel(10, sq_sigma_twice));
	}

	/* create MMD instance */
	CLinearTimeMMD* mmd=new CLinearTimeMMD(combined_kernel, streaming_p,
			streaming_q, m);

	/* kernel selection instance with regularisation term */
	CMMDKernelSelectionOptSingle* selection=
			new CMMDKernelSelectionOptSingle(mmd, 10E-5);

	/* start streaming features parser */
	streaming_p->start_parser();
	streaming_q->start_parser();

	SGVector<float64_t> ratios=selection->compute_measures();
	ratios.display_vector("ratios");

	/* assert weights against matlab */
//	ratios =
//	   0.947668253683719
//	   0.336041393822230
//	   0.093824478467851
	ASSERT(CMath::abs(ratios[0]-0.947668253683719)<10E-15);
	ASSERT(CMath::abs(ratios[1]-0.336041393822230)<10E-15);
	ASSERT(CMath::abs(ratios[2]-0.093824478467851)<10E-15);

	/* start streaming features parser */
	streaming_p->end_parser();
	streaming_q->end_parser();

	SG_UNREF(selection);
}

void test_kernel_choice_linear_time_mmd_maxmmd_single()
{
	index_t m=8;
	index_t d=3;
	SGMatrix<float64_t> data(d,2*m);
	for (index_t i=0; i<2*d*m; ++i)
		data.matrix[i]=i;

	/* create data matrix for each features (appended is not supported) */
	SGMatrix<float64_t> data_p(d, m);
	memcpy(&(data_p.matrix[0]), &(data.matrix[0]), sizeof(float64_t)*d*m);

	SGMatrix<float64_t> data_q(d, m);
	memcpy(&(data_q.matrix[0]), &(data.matrix[d*m]), sizeof(float64_t)*d*m);

	/* normalise data to get some reasonable values for Q matrix */
	float64_t max_p=data_p.max_single();
	float64_t max_q=data_q.max_single();

	SG_SPRINT("%f, %f\n", max_p, max_q);

	for (index_t i=0; i<d*m; ++i)
	{
		data_p.matrix[i]/=max_p;
		data_q.matrix[i]/=max_q;
	}

	data_p.display_matrix("data_p");
	data_q.display_matrix("data_q");

	CDenseFeatures<float64_t>* features_p=new CDenseFeatures<float64_t>(data_p);
	CDenseFeatures<float64_t>* features_q=new CDenseFeatures<float64_t>(data_q);

	/* create stremaing features from dense features */
	CStreamingFeatures* streaming_p=
			new CStreamingDenseFeatures<float64_t>(features_p);
	CStreamingFeatures* streaming_q=
			new CStreamingDenseFeatures<float64_t>(features_q);

	/* create kernels with sigmas 2^5 to 2^7 */
	CCombinedKernel* combined_kernel=new CCombinedKernel();
	for (index_t i=5; i<=7; ++i)
	{
		/* shoguns kernel width is different */
		float64_t sigma=CMath::pow(2, i);
		float64_t sq_sigma_twice=sigma*sigma*2;
		combined_kernel->append_kernel(new CGaussianKernel(10, sq_sigma_twice));
	}

	/* create MMD instance */
	CLinearTimeMMD* mmd=new CLinearTimeMMD(combined_kernel, streaming_p,
			streaming_q, m);

	/* kernel selection instance */
	CMMDKernelSelectionMax* selection=
			new CMMDKernelSelectionMax(mmd);

	/* start streaming features parser */
	streaming_p->start_parser();
	streaming_q->start_parser();

	/* assert that the correct kernel is returned since I checked the MMD
	 * already very often */
	CKernel* result=selection->select_kernel();
	CGaussianKernel* casted=dynamic_cast<CGaussianKernel*>(result);
	ASSERT(casted);

	/* assert weights against matlab */
	CKernel* reference=combined_kernel->get_first_kernel();
	ASSERT(result==reference);
	SG_UNREF(reference);

	/* start streaming features parser */
	streaming_p->end_parser();
	streaming_q->end_parser();

	SG_UNREF(selection);
	SG_UNREF(result);
}

void test_kernel_choice_quadratic_time_mmd_maxmmd_single()
{
	index_t m=8;
	index_t d=3;
	SGMatrix<float64_t> data(d,2*m);
	for (index_t i=0; i<2*d*m; ++i)
		data.matrix[i]=i;

	/* create data matrix for each features (appended is not supported) */
	SGMatrix<float64_t> data_p(d, m);
	memcpy(&(data_p.matrix[0]), &(data.matrix[0]), sizeof(float64_t)*d*m);

	SGMatrix<float64_t> data_q(d, m);
	memcpy(&(data_q.matrix[0]), &(data.matrix[d*m]), sizeof(float64_t)*d*m);

	/* normalise data to get some reasonable values for Q matrix */
	float64_t max_p=data_p.max_single();
	float64_t max_q=data_q.max_single();

	SG_SPRINT("%f, %f\n", max_p, max_q);

	for (index_t i=0; i<d*m; ++i)
	{
		data_p.matrix[i]/=max_p;
		data_q.matrix[i]/=max_q;
	}

	data_p.display_matrix("data_p");
	data_q.display_matrix("data_q");

	CDenseFeatures<float64_t>* features_p=new CDenseFeatures<float64_t>(data_p);
	CDenseFeatures<float64_t>* features_q=new CDenseFeatures<float64_t>(data_q);

	/* create kernels with sigmas 2^5 to 2^7 */
	CCombinedKernel* combined_kernel=new CCombinedKernel();
	for (index_t i=5; i<=7; ++i)
	{
		/* shoguns kernel width is different */
		float64_t sigma=CMath::pow(2, i);
		float64_t sq_sigma_twice=sigma*sigma*2;
		combined_kernel->append_kernel(new CGaussianKernel(10, sq_sigma_twice));
	}

	/* create MMD instance, convienience constructor */
	CQuadraticTimeMMD* mmd=new CQuadraticTimeMMD(combined_kernel, features_p,
			features_q);

	/* kernel selection instance */
	CMMDKernelSelectionMax* selection=
			new CMMDKernelSelectionMax(mmd);

	/* assert correct mmd values, maxmmd criterion is already checked with
	 * linear time mmd maxmmd selection. Do biased and unbiased m*MMD */

	/* unbiased m*MMD */
	SGVector<float64_t> measures=selection->compute_measures();
	measures.display_vector("unbiased mmd");
//	unbiased_quad_mmds =
//	   0.001164382204818   0.000291185913881   0.000072802127661
	ASSERT(CMath::abs(measures[0]-0.001164382204818)<10E-15);
	ASSERT(CMath::abs(measures[1]-0.000291185913881)<10E-15);
	ASSERT(CMath::abs(measures[2]-0.000072802127661)<10E-15);

	/* biased m*MMD */
	mmd->set_statistic_type(BIASED);
	measures=selection->compute_measures();
	measures.display_vector("biased mmd");
//	biased_quad_mmds =
//	   0.001534961982492   0.000383849322208   0.000095969134022
	ASSERT(CMath::abs(measures[0]-0.001534961982492)<10E-15);
	ASSERT(CMath::abs(measures[1]-0.000383849322208)<10E-15);
	ASSERT(CMath::abs(measures[2]-0.000095969134022)<10E-15);

	/* since convienience constructor was use for mmd, features have to be
	 * cleaned up by hand */
	SG_UNREF(features_p);
	SG_UNREF(features_q);

	SG_UNREF(selection);
}

int main(int argc, char** argv)
{
	init_shogun_with_defaults();
//	sg_io->set_loglevel(MSG_DEBUG);

	/* linear time mmd */
	test_kernel_choice_linear_time_mmd_maxmmd_single();
	test_kernel_choice_linear_time_mmd_opt_single();
	test_kernel_choice_linear_time_mmd_opt_comb();
	test_kernel_choice_linear_time_mmd_maxl2_comb();

	/* quadratic time mmd */
	test_kernel_choice_quadratic_time_mmd_maxmmd_single();


	exit_shogun();
	return 0;
}

