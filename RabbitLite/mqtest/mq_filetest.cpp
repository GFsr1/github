#include "../mqcommon/mq_helper.hpp"

int main() {
    // zkpmq::FileHelper helper("../mqcommon/logger.hpp");
    // DLOG("是否存在：%d", helper.exists());
    // DLOG("文件大小：%ld", helper.size());

    // zkpmq::FileHelper tmp_helper("./aaa/bbb/ccc/tmp.txt");
    // if (tmp_helper.exists() == false) {
    //     std::string path = zkpmq::FileHelper::parentDirectory("./aaa/bbb/ccc/tmp.txt");
    //     if (zkpmq::FileHelper(path).exists() == false) {
    //         zkpmq::FileHelper::createDirectory(path);
    //     }
    //     zkpmq::FileHelper::createFile("./aaa/bbb/ccc/tmp.txt");
    // }

    // std::string body;
    // helper.read(body);
    // DLOG("%s", body.c_str());
    // tmp_helper.write(body);
    // zkpmq::FileHelper tmp_helper("./aaa/bbb/ccc/tmp.txt");
    // char str[16] = {0};
    // tmp_helper.read(str, 8, 11);
    // DLOG("[%s]", str);
    // tmp_helper.write("12345678901", 8, 11);
    // tmp_helper.rename("./aaa/bbb/ccc/test.txt");

    // zkpmq::FileHelper::removeFile("./aaa/bbb/ccc/test.txt");
    // zkpmq::FileHelper::removeDirectory("./aaa");
    
    return 0;
}