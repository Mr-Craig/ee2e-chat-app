#include "../../ChatBackend/Utils/Utils.h"

int main() {
    std::string hashedPassword = utils::hashPassword("password");
    return !(hashedPassword.size() == HASHLEN);
}