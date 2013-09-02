#ifndef _H_BCI_FSL_H_
#define _H_BCI_FSL_H_

#include <configparser/configparser.h>

#include <visionsystem/plugin.h>
#include <vision/image/image.h>

#include <boost/thread.hpp>

#include <bci-interface/BCIInterface.h>

namespace visionsystem
{

class BCIVSCore : public Plugin
{
    public:
        BCIVSCore( VisionSystem * vs, std::string sandbox );
        ~BCIVSCore();

        bool pre_fct();
        void preloop_fct();
        void loop_fct();
        bool post_fct();

    private:
        void AddArrows();
        void InterfaceThread();

        unsigned int width;
        unsigned int height;
        bool fullscreen;
        unsigned int iwidth;
        unsigned int iheight;

        bciinterface::BCIInterface * m_interface;
        boost::thread * m_interface_thread;

        Camera * cam_;
};

} // namespace visionsystem

#endif
