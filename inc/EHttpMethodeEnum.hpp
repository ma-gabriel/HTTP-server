#ifndef EHTTPMETHODENUM_HPP
# define EHTTPMETHODENUM_HPP

enum EHttpMethode
{
    Post,
    Get,
    Delete,
    Put
};

std::string getMethodString(EHttpMethode method);

#endif