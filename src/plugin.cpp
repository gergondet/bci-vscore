#include <boost/asio.hpp>
#include "plugin.h"

#include <bci-interface/Background/BufferBG.h>
#include <bci-interface/DisplayObject/SSVEPStimulus.h>
#include <bci-interface/CommandReceiver/UDPReceiver.h>
#include <bci-interface/CommandInterpreter/SimpleInterpreter.h>
#include <bci-interface/CommandOverrider.h>
#include <SFML/Graphics.hpp>

#include <algorithm>

using namespace bciinterface;

namespace visionsystem
{

    BCIVSCore::BCIVSCore( VisionSystem * vs, std::string sandbox )
    : Plugin(vs, "bci-vscore", sandbox),
      width(1024), height(768), fullscreen(false),
      iwidth(800), iheight(600),
      m_interface(0),
      m_interface_thread(0), 
      cam_(0)
    {
    }

    BCIVSCore::~BCIVSCore()
    {
        if(m_interface_thread)
        {
            delete m_interface_thread;
        }
    }

    bool BCIVSCore::pre_fct()
    {
        cam_ = get_default_camera();
        register_to_cam< vision::Image<uint32_t, vision::RGB> >(cam_, 10);

        return true;
    }

    void BCIVSCore::preloop_fct()
    {
        m_interface_thread = new boost::thread(boost::bind(&BCIVSCore::InterfaceThread, this));
        sleep(3);
    }

    void BCIVSCore::loop_fct()
    {
        vision::Image<uint32_t, vision::RGB> * img = dequeue_image< vision::Image<uint32_t, vision::RGB> > (cam_);

        if(m_interface)
        {
            BufferBG * bg = dynamic_cast<BufferBG *>(m_interface->GetBackground());
            if(bg)
            {
                bg->UpdateFromBuffer_RGB((unsigned char *)img->raw_data);
            }
        }

        enqueue_image(cam_, img);
    }

    bool BCIVSCore::post_fct()
    {
        unregister_to_cam< vision::Image<uint32_t, vision::RGB> > (cam_);

        if(m_interface)
        {
            m_interface->Close();
        }
        m_interface_thread->join();

        return true;
    }

    void BCIVSCore::AddArrows()
    {
        m_interface->AddObject(new SSVEPStimulus(6, 60, width/2, 100, 200,200, get_sandbox() + "/data/UP.png", get_sandbox() + "/data/UP_HL.png"));
        if(width == iwidth)
        {
            m_interface->AddObject(new SSVEPStimulus(8, 60, width-100, height/2, 200, 200, get_sandbox() + "/data/RIGHT.png", get_sandbox() + "/data/RIGHT_HL.png"));
        }
        else
        {
            m_interface->AddObject(new SSVEPStimulus(8, 60, (width + iwidth)/2 + 100, height/2, 200, 200, get_sandbox() + "/data/RIGHT.png", get_sandbox() + "/data/RIGHT_HL.png"));
        }
        m_interface->AddObject(new SSVEPStimulus(10, 60, width/2, height-100, 200, 200, get_sandbox() + "/data/DOWN.png", get_sandbox() + "/data/DOWN_HL.png"));
        if(width == iwidth)
        {
            m_interface->AddObject(new SSVEPStimulus(14, 60, 100, height/2,200, 200, get_sandbox() + "/data/LEFT.png", get_sandbox() + "/data/LEFT_HL.png"));
        }
        else
        {
            m_interface->AddObject(new SSVEPStimulus(14, 60, (width - iwidth)/2 - 100, height/2,200, 200, get_sandbox() + "/data/LEFT.png", get_sandbox() + "/data/LEFT_HL.png"));
        }
    }

    void BCIVSCore::InterfaceThread()
    {
        std::cout << "Creating an interface, size: " << width << "x" << height << std::endl;
        if(fullscreen)
        {
            std::cout << "Fullscreen is on" << std::endl;
        }
        else
        {
            std::cout << "Fullscreen is off" << std::endl;
        }
        m_interface = new BCIInterface(width, height);
        UDPReceiver * receiver = new UDPReceiver(2222);
        m_interface->SetCommandReceiver(receiver);
        
        m_interface->SetBackground(new BufferBG(640, 480, width, height, iwidth, iheight));

        CommandOverrider overrider;
        m_interface->SetCommandOverrider(&overrider);

        int out_cmd = -1;
        float timeout = 2;
        sf::RenderWindow * app = 0;
        boost::thread th(boost::bind(&bciinterface::BCIInterface::DisplayLoop, m_interface, app, fullscreen, &out_cmd, timeout));

        SimpleInterpreter * interpreter = new SimpleInterpreter();

        while(out_cmd != 0)
        {
            m_interface->StartParadigm();
            overrider.Clean();
            overrider.AddOverrideCommand(sf::Keyboard::Up, 1);
            overrider.AddOverrideCommand(sf::Keyboard::Right, 2);
            overrider.AddOverrideCommand(sf::Keyboard::Down, 3);
            overrider.AddOverrideCommand(sf::Keyboard::Left, 4);
            overrider.AddOverrideCommand(sf::Keyboard::S, 5);
            AddArrows();
            m_interface->AddObject(new SSVEPStimulus(9, 60, 100, height - 100, 200, 200, get_sandbox() + "/data/SWITCH.png", get_sandbox() + "/data/SWITCH_HL.png"));
            m_interface->SetCommandInterpreter(interpreter);
            while(m_interface->ParadigmStatus())
            {
                usleep(1000);
            }
            m_interface->Clean();
            m_interface->SetCommandInterpreter(0);
        }

        m_interface->Close();
        BCIInterface * intf = m_interface;
        m_interface = 0;
        delete interpreter;
        delete intf;
        whiteboard_write("core_stop", true);
    }

} // namespace visionsystem

PLUGIN(visionsystem::BCIVSCore)
