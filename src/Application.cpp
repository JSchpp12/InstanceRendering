#include "Application.hpp"

using namespace star; 

int Application::disabledLightCounter = int(0);
bool Application::upCounter = true;
bool Application::rotatingCounterClock = true;

std::chrono::steady_clock::time_point Application::timeSinceLastUpdate = std::chrono::steady_clock::now();

Application::Application(star::StarScene& scene) : StarApplication(scene)
{
    this->camera.setPosition(glm::vec3{ 2.0, 1.0f, 3.0f });
    auto camPosition = this->camera.getPosition();
    this->camera.setLookDirection(-camPosition);

    auto mediaDirectoryPath = star::ConfigFile::getSetting(star::Config_Settings::mediadirectory);
    {
        auto lionPath = star::ConfigFile::getSetting(star::Config_Settings::mediadirectory) + "models/lion-statue/source/rapid.obj";
        auto materialsPath = mediaDirectoryPath + "models/lion-statue/source";
        
        auto lion = BasicObject::New(lionPath);
        const int width = 25; 
        const float mvmt = 0.3f; 
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                auto& lion_f = lion->createInstance();
                lion_f.setScale(glm::vec3{ 0.01f, 0.01f, 0.01f });
                lion_f.setPosition(glm::vec3{ 0.0, 0.0, 0.0 });
                lion_f.rotateGlobal(star::Type::Axis::x, -90);
                lion_f.moveRelative(glm::vec3{ mvmt * i, 0.0, mvmt * j});
            }
        }

        this->scene.add(std::move(lion)); 
        this->scene.add(std::make_unique<star::Light>(star::Type::Light::point, glm::vec3{ 0, 1, 0 }));
        this->scene.add(std::make_unique<star::Light>(star::Type::Light::point, glm::vec3{ 2, 1, 2 }, glm::vec4{1.0, 0.0, 0.0, 1.0},
            glm::vec4{ 1.0, 0.0, 0.0, 1.0 }, glm::vec4{1.0, 0.0, 0.0, 1.0}));
        this->scene.add(std::make_unique<star::Light>(star::Type::Light::point, glm::vec3{ 4, 1, 4 }, glm::vec4{ 0.0, 1.0, 0.0, 1.0 },
            glm::vec4{ 0.0, 1.0, 0.0, 1.0 }, glm::vec4{ 0.0, 1.0, 0.0, 1.0 }));
        this->scene.add(std::make_unique<star::Light>(star::Type::Light::point, glm::vec3{ 6, 1, 6 }, glm::vec4{ 0.0, 0.0, 1.0, 1.0 },
            glm::vec4{ 0.0, 0.0, 1.0, 1.0 }, glm::vec4{ 0.0, 0.0, 1.0, 1.0 }));
        this->scene.add(std::make_unique<star::Light>(star::Type::Light::directional, glm::vec3{10, 10, 10}));
    }

    std::cout << "App Controls" << std::endl;
    std::cout << "Press M to switch lights off and on in order" << std::endl;
    std::cout << "Right Arrow: rotate sun light clockwise" << std::endl;
    std::cout << "Left Arrow: rotate sun light counter clockwise" << std::endl;
    std::cout << "Up Arrow: increase diameter of spot light" << std::endl;
    std::cout << "Down Arrow: decrease diameter of spot light" << std::endl;
    std::cout << "U: Increase outer diam of spot light" << std::endl;
    std::cout << "Y: Decrease outer diam of spot light" << std::endl;
    std::cout << "J: Increase inner diam of spot light" << std::endl;
    std::cout << "H: Decrease inner diam of spot light" << std::endl;
    std::cout << "Z: Enable/Disable sun" << std::endl;
}

void Application::Load()
{
}

void Application::Update()
{

}

void Application::onKeyPress(int key, int scancode, int mods)
{

}

void Application::onKeyRelease(int key, int scancode, int mods)
{
}

void Application::onMouseMovement(double xpos, double ypos)
{
}

void Application::onMouseButtonAction(int button, int action, int mods)
{
}

void Application::onScroll(double xoffset, double yoffset)
{
}

void Application::onWorldUpdate()
{
}
