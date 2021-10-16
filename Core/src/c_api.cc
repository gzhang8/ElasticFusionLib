//
// Created by gzhang8 on 6/3/20.
//

#include "ElasticFusion.h"



// Note: how color is defined is changed


extern "C"
{
    void* create_elastic_fustion(
            int w,
            int h,
            float fx,
            float fy,
            float cx,
            float cy,
            int timeDelta,
            int countThresh,
            float errThresh,
            float covThresh,
            int closeLoops, // bool
            int iclnuim,  // bool
            int reloc, // bool
            float photoThresh,
            float confidence,
            float depthCut,
            float icpThresh,
            int fastOdom,       // bool
            float fernThresh,
            int so3, // bool
            int frameToFrameRGB, // bool
            char* fileName
            ){

        Resolution::getInstance(w, h);
        Intrinsics::getInstance(fx, fy, cx, cy);
//        ElasticFusion(const int timeDelta = 200,
//        const int countThresh = 35000,
//        const float errThresh = 5e-05,
//        const float covThresh = 1e-05,
//        const bool closeLoops = true,
//        const bool iclnuim = false,
//        const bool reloc = false,
//        const float photoThresh = 115,
//        const float confidence = 10,
//        const float depthCut = 3,
//        const float icpThresh = 10,
//        const bool fastOdom = false,
//        const float fernThresh = 0.3095,
//        const bool so3 = true,
//        const bool frameToFrameRGB = false,
//        const std::string fileName = "");
        ElasticFusion* efusion_ptr = new ElasticFusion(
                timeDelta,
                countThresh,
                errThresh,
                covThresh,
                (closeLoops!=0),
                (iclnuim!=0),
                (reloc!=0),
                photoThresh,
                confidence,
                depthCut,
                icpThresh,
                (fastOdom!=0),
                fernThresh,
                so3,
                (frameToFrameRGB!=0),
                std::string(fileName)
                );
        return (void*)efusion_ptr;

    }

    void delete_elastic_fusion(
            void* efusion_void_ptr
    ) {

        ElasticFusion *efusion_ptr = (ElasticFusion *) efusion_void_ptr;
        delete efusion_ptr;

    }


//    EFUSION_API void processFrame(const unsigned char * rgb,
//                                  const unsigned short * depth,
//                                  const int64_t & timestamp,
//                                  const Eigen::Matrix4f * inPose = 0,
//                                  const float weightMultiplier = 1.f,
//                                  const bool bootstrap = false);

    /**
    * Process an rgb/depth map pair
    * @param rgb unsigned char row major order
    * @param depth unsigned short z-depth in millimeters, invalid depths are 0
    * @param timestamp nanoseconds (actually only used for the output poses, not important otherwise)
    * @param inPose optional input SE3 pose (if provided, we don't attempt to perform tracking)
    * @param weightMultiplier optional full frame fusion weight
    * @param bootstrap if true, use inPose as a pose guess rather than replacement
    */
    void process_frame(
            void* efusion_void_ptr,
            unsigned char * rgb,
            unsigned short * depth,
            int64_t timestamp,
            float depth_factor,
            float* in_pose_col_major,
            float weightMultiplier,
            int bootstrap // bool
            )

    {
        ElasticFusion* efusion_ptr = (ElasticFusion*)efusion_void_ptr;

        if (in_pose_col_major != NULL){
            Eigen::Map<Eigen::Matrix<float,4,4,Eigen::ColMajor> >inPose_map(in_pose_col_major);
            Eigen::Matrix4f inPose(inPose_map);
            efusion_ptr->processFrame(rgb, depth, timestamp, depth_factor, &inPose, weightMultiplier, (bootstrap!=0));
        } else {
            efusion_ptr->processFrame(rgb, depth, timestamp, depth_factor, NULL, weightMultiplier, (bootstrap!=0));
        }

    }


    void render(
            void* efusion_void_ptr,
            double* mvp_ptr,
            float threshold,
            int drawUnstable,//bool
            int drawNormals, //bool
            int drawColors,  //bool
            int drawPoints,  //bool
            int drawWindow,  //bool
            int drawTimes,   //bool
            int time,
            int timeDelta){
        //        EFUSION_API void renderPointCloud(pangolin::OpenGlMatrix mvp,
        //                                          const float threshold,
        //                                          const bool drawUnstable,
        //                                          const bool drawNormals,
        //                                          const bool drawColors,
        //                                          const bool drawPoints,
        //                                          const bool drawWindow,
        //                                          const bool drawTimes,
        //                                          const int time,
        //                                          const int timeDelta);
        ElasticFusion* efusion_ptr = (ElasticFusion*)efusion_void_ptr;
        pangolin::OpenGlMatrix mvp = pangolin::OpenGlMatrix::ColMajor4x4(mvp_ptr);


        GlobalModel& model = efusion_ptr->getGlobalModel();

        model.renderPointCloud(
                mvp,
                threshold,
                drawUnstable!=0,
                drawNormals!=0,
                drawColors!=0,
                drawPoints!=0,
                drawWindow!=0,
                drawTimes!=0,
                time,
                timeDelta
                );
    }


    void save_ply(
            void* efusion_void_ptr,
            char* ply_fpath,
            float save_confidence){
        ElasticFusion* efusion_ptr = (ElasticFusion*)efusion_void_ptr;
        efusion_ptr->savePly(std::string(ply_fpath), save_confidence);
    }

} // extern "C"
