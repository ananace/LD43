#include "Application.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char** argv)
{
    Application app;

    app.init(argc, argv);

    try
    {
        app.run();
    }
    catch (const std::runtime_error& ex)
    {
        std::cerr << "Exception occured during execution;" << std::endl << ex.what() << std::endl;
        throw ex;
    }

    return 0;
}
