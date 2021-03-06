#include "imagefunctions.h"
#include "makezmg.h"

unsigned long long GetTotalBytes(std::string instr)
{
    unsigned long long totbyt = 0;
    int infile = open(instr.c_str(), O_RDONLY);
    ioctl(infile, BLKGETSIZE64, &totbyt);
    close(infile);
    return totbyt;
}

void StartImaging(std::string instring, std::string outpath, std::string outstr, int radio) 
{
    if(radio == 0) // RAW
    {
        ReadBytes(instring, std::string(outpath + "/" + outstr + ".dd"));
        printf("Raw Forensic Image Finished Successfully.\n");
    }
    else if(radio == 1) // AFF4
    {
        ReadBytes(instring, std::string(outpath + "/" + outstr + ".dd"));
        std::string aff4cmd = std::string(getenv("HOME")) + std::string("/.local/share/wombatforensics/aff4imager");
        aff4cmd += " -i " + outpath + "/" + outstr + ".dd -i " + outpath + "/" + outstr + ".dd.log -o " + outpath + "/" + outstr + ".aff4";
        system(aff4cmd.c_str());
        std::remove(std::string(outpath + "/" + outstr + ".dd").c_str());
        printf("AFF4'd Forensic Image Finished Successfully.\n");
    }
    else if(radio == 2) // SFS
    {
        ReadBytes(instring, std::string(outpath + "/" + outstr + ".dd"));
        std::string sqshcmd = "mksquashfs " + outpath + "/" + outstr + ".dd " + outpath + "/" + outstr + ".dd.log " + outpath + "/" + outstr + ".sfs";
        system(sqshcmd.c_str());
        std::remove(std::string(outpath + "/" + outstr + ".dd").c_str());
        std::remove(std::string(outpath + "/" + outstr + ".dd.log").c_str());
        printf("Squashfs'd Forensic Image Finished Successfully.\n");
    }
    else if(radio == 3) // ZMG
    {
        ReadBytes(instring, std::string(outpath + "/" + outstr + ".dd"));
        printf("outpath: %s", outpath.c_str());
        mkdir(std::string(outpath + "/tmp").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        rename(std::string(outpath + "/" + outstr + ".dd").c_str(), std::string(outpath + "/tmp/" + outstr + ".dd").c_str());
        rename(std::string(outpath + "/" + outstr + ".dd.log").c_str(), std::string(outpath + "/tmp/" + outstr + ".dd.log").c_str());
        MakeZmg(std::string(outpath + "/tmp"), std::string(outpath + "/" + outstr + ".zmg"));
        std::remove(std::string(outpath + "/tmp/" + outstr + ".dd").c_str());
        std::remove(std::string(outpath + "/tmp/" + outstr + ".dd.log").c_str());
        std::remove(std::string(outpath + "/tmp").c_str());
    }
}


void ReadBytes(std::string instring, std::string outstr)
{
    std::ofstream logfile;
    std::size_t lpar = instring.find("(");
    std::size_t rpar = instring.find(")");
    std::string instr = instring.substr(lpar+1, rpar-lpar-1);
    lpar = instring.find("[");
    rpar = instring.find("]");
    std::string serialnumber = instring.substr(lpar+6, rpar-lpar-6);
    lpar = instring.find("[");
    std::string deviceinfo = instring.substr(0, lpar-1);

    time_t starttime = time(NULL);
    logfile.open(outstr + ".log", std::ofstream::out | std::ofstream::app);
    char buff[35];
    logfile << "Wombat Forensics v0.3 Forensic Image started at: " << GetDateTime(buff) << "\n";
    unsigned long long totalbytes = 0;
    unsigned int sectorsize = 512;
    unsigned long long curpos = 0;
    int infile = open(instr.c_str(), O_RDONLY);
    int outfile = open(outstr.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_APPEND, S_IRWXU);
    ioctl(infile, BLKGETSIZE64, &totalbytes);
    ioctl(infile, BLKSSZGET, &sectorsize);
    logfile << "\nSource Device\n";
    logfile << "-------------\n";
    logfile << "Device: " << deviceinfo << "\n";
    logfile << "Device Path: " << instr << "\n";
    logfile << "Serial Number: " << serialnumber << "\n";
    logfile << "Size: " << totalbytes << " bytes\n";
    logfile << "Block Size: " << sectorsize << " bytes\n";
    lseek(infile, 0, SEEK_SET);
    lseek(outfile, 0, SEEK_SET);
    unsigned char c[MD5_DIGEST_LENGTH];
    unsigned char o[MD5_DIGEST_LENGTH];
    int i;
    MD5_CTX mdcontext;
    MD5_CTX outcontext;
    MD5_Init(&mdcontext);
    MD5_Init(&outcontext);
    while (curpos < totalbytes)
    {
        char bytebuf[sectorsize];
        char imgbuf[sectorsize];
	ssize_t bytesread = read(infile, bytebuf, sectorsize);
	ssize_t byteswrite = write(outfile, bytebuf, sectorsize);
        if(bytesread == -1)
            perror("Read Error:");
	if(byteswrite == -1)
	    perror("Write Error:");
        MD5_Update(&mdcontext, bytebuf, bytesread);
        ssize_t byteswrote = pread(outfile, imgbuf, sectorsize, curpos);
        MD5_Update(&outcontext, imgbuf, byteswrote);
	curpos = curpos + sectorsize;
	printf("Wrote %lld out of %lld bytes\r", curpos, totalbytes);
	fflush(stdout);
    }
    MD5_Final(c, &mdcontext);
    MD5_Final(o, &outcontext);
    time_t endtime = time(NULL);
    logfile << "Wrote " << curpos << " out of " << totalbytes << " bytes\n";
    logfile << "Forensic Image: " << outstr << "\n";
    logfile << "Forensic Image finished at: " << GetDateTime(buff) << "\n";
    logfile << "Forensic Image created in: " << difftime(endtime, starttime) << " seconds\n\n";
    printf("\nForensic Image Creation Finished!\n");
    std::stringstream srcstr;
    std::stringstream imgstr;
    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        srcstr << std::hex << (int)c[i];
	printf("%02x", c[i]);
    }
    printf(" - MD5 Source Device\n");
    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        imgstr << std::hex << (int)o[i];
	printf("%02x", o[i]);
    }
    printf(" - MD5 Forensic Image\n");
    std::string srcmd5 = "";
    std::string imgmd5 = "";
    srcmd5 = srcstr.str();
    imgmd5 = imgstr.str();
    logfile << srcmd5 << " - MD5 Source Device\n";
    logfile << imgmd5 << " - MD5 Forensic Image\n";
    if(srcmd5.compare(imgmd5) == 0)
    {
	printf("Verification Successful\n");
        logfile << "\nVerification Successful\n";
    }
    else
    {
	printf("Verification Failed\n");
        logfile << "Verification Failed\n";
    }
    logfile << "\nFinished Forensic Image Verification at " << GetDateTime(buff) << "\n";
    logfile << "Finished Forensic Image Verification in: " << difftime(endtime, starttime) << " seconds\n";
    logfile.close();
    close(infile);
    close(outfile);
}

std::string Verify(QString outstr)
{
    std::string mntoutstr = "";
    if(outstr.toLower().endsWith(".zmg"))
        mntoutstr = "/tmp/wombatforensics/datamnt/" + outstr.split("/").last().toLower().split(".zmg").first().toStdString() + ".dd";
    else if(outstr.toLower().endsWith(".e01"))
        mntoutstr = "/tmp/wombatforensics/datamnt/" + outstr.split("/").last().toStdString() + ".raw";
    else if(outstr.toLower().endsWith(".aff"))
        mntoutstr = "/tmp/wombatforensics/datamnt/" + outstr.split("/").last().toStdString() + ".raw";
    else if(outstr.toLower().endsWith(".sfs"))
        mntoutstr = "/tmp/wombatforensics/datamnt/" + outstr.split("/").last().toLower().split(".sfs").first().toStdString() + ".dd";
    printf("outstr: %s\n", outstr.toStdString().c_str());
    printf("mntoutstr: %s\n", mntoutstr.c_str());
    
    std::size_t rfound = outstr.toStdString().rfind("/");
    std::string imgname = outstr.toStdString().substr(rfound+1);
    std::ifstream infile(mntoutstr + ".log");
    std::string md5hash = "";
    std::string curstr = "";
    while(std::getline(infile, curstr))
    {
        std::size_t found = curstr.find(" - MD5 Forensic Image");
        if(found != std::string::npos)
            md5hash = curstr.substr(0, found);
    }
    if(md5hash.empty())
    {
        if(outstr.toLower().endsWith(".e01"))
        {
            // implement EWF GET MD5 here...
            libewf_handle_t* ewfhandle = NULL;
            libewf_error_t* ewferror = NULL;
            char* filenames[1] = {NULL};
            char* iname = new char[outstr.toStdString().size() + 1];
            strcpy(iname, outstr.toStdString().c_str());
            filenames[0] = (char*)iname;
            libewf_handle_initialize(&ewfhandle, &ewferror);
            libewf_handle_open(ewfhandle, filenames, 1, LIBEWF_OPEN_READ, &ewferror);
            uint8_t md5_hash[32];
            libewf_handle_get_utf8_hash_value_md5(ewfhandle, md5_hash, 32, &ewferror);
            md5hash = QString::fromUtf8(reinterpret_cast<char*>(md5_hash)).toStdString();
            libewf_handle_close(ewfhandle, &ewferror);
            libewf_handle_free(&ewfhandle, &ewferror);
            //printf("MD5 hash from E01 file: %s\n", md5hash.c_str());
        }
        else if(outstr.toLower().endsWith(".aff"))
        {
            // implement AFF GET MD5 here...
            char* iname = new char[outstr.toStdString().size() + 1];
            strcpy(iname, outstr.toStdString().c_str());
            static AFFILE* afimage = NULL;
            afimage = af_open(iname, O_RDONLY|O_EXCL,0);
	    unsigned char md5_stored[16];
	    size_t md5len = sizeof(md5_stored);
            af_get_seg(afimage, AF_MD5, 0, md5_stored, &md5len);
            std::stringstream ss;
            for(int i=0; i < 16; ++i)
                ss << std::hex << (int)md5_stored[i];
            md5hash = ss.str();
            af_close(afimage);
            //printf("MD5 hash from AFF file: %s\n", md5hash.c_str());
        }
    }
    //printf("original md5: %s\n", md5hash.c_str());
    infile.close();
    unsigned long long totalbytes = 0;
    int ofile = open(mntoutstr.c_str(), O_RDONLY);
    ioctl(ofile, BLKGETSIZE64, &totalbytes);
    close(ofile);
    //std::ofstream logfile;
    time_t starttime = time(NULL);
    char buff[35];
    //logfile.open(outstr + ".log", std::ofstream::out | std::ofstream::app);
    //logfile << "\nStarting Image Verification at " << GetDateTime(buff) << "\n";
    unsigned int sectorsize = 512;
    unsigned char o[MD5_DIGEST_LENGTH];
    int i;
    FILE* outfile = fopen(mntoutstr.c_str(), "rb");
    MD5_CTX outcontext;
    int obytes;
    unsigned char odata[sectorsize];
    MD5_Init(&outcontext);
    unsigned long long curpos = 0;
    while((obytes = fread(odata, 1, sectorsize, outfile)) != 0)
    {
        curpos = curpos + sectorsize;
        MD5_Update(&outcontext, odata, obytes);
        printf("Bytes Read: %lld/%lld\r", curpos, totalbytes);
        fflush(stdout);
    }
    //logfile << "Bytes Read: " << curpos << "/" << totalbytes << "\n\n";
    MD5_Final(o, &outcontext);
    std::stringstream imgstr;
    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        imgstr << std::hex << (int)o[i];
	printf("%02x", o[i]);
    }
    printf(" - MD5 Forensic Image\n");
    std::string imgmd5 = "";
    imgmd5 = imgstr.str();
    //logfile << imgmd5 << " - MD5 Forensic Image\n\n";
    
    imgname += " Verification ";
    if(md5hash.compare(imgmd5) == 0)
    {
	printf("Verification Successful\n");
        //logfile << "Verification Successful\n";
        imgname += "Successful";
    }
    else if(md5hash.empty())
    {
        printf("Manual MD5 Hash Verification Comparison Required\n");
        //logfile << "Verification Comparison Required\n";
        imgname += "Requires Manual Comparison";
    }
    else
    {
	printf("Verification Failed\n");
        //logfile << "Verification Failed\n";
        imgname += "Failed";
    }
    time_t endtime = time(NULL);
    //logfile << "\nFinished Forensic Image Verification at " << GetDateTime(buff) << "\n";
    //logfile << "Finished Forensic Image Verification in: " << difftime(endtime, starttime) << " seconds\n";
    //logfile.close();
    fclose(outfile);

    return imgname;
}
/*
 *#include <stdio.h>
 #include <openssl/evp.h>

 main(int argc, char *argv[])
 {
 EVP_MD_CTX *mdctx;
 const EVP_MD *md;
 char mess1[] = "Test Message\n";
 char mess2[] = "Hello World\n";
 unsigned char md_value[EVP_MAX_MD_SIZE];
 int md_len, i;

 OpenSSL_add_all_digests();

 if(!argv[1]) {
        printf("Usage: mdtest digestname\n");
        exit(1);
 }

 md = EVP_get_digestbyname(argv[1]); // will be md = EVP_MD5();
 // may switch to input if the user wants to calculate md5, sha1, or sha256

 if(!md) {
        printf("Unknown message digest %s\n", argv[1]);
        exit(1);
 }

 mdctx = EVP_MD_CTX_create();
 EVP_DigestInit_ex(mdctx, md, NULL);
 EVP_DigestUpdate(mdctx, mess1, strlen(mess1));
 EVP_DigestUpdate(mdctx, mess2, strlen(mess2));
 EVP_DigestFinal_ex(mdctx, md_value, &md_len);
 EVP_MD_CTX_destroy(mdctx);

 printf("Digest is: ");
 for(i = 0; i < md_len; i++)
        printf("%02x", md_value[i]);
 printf("\n");

 // Call this once before exit.
 EVP_cleanup();
 exit(0);
 }
 *
 */ 
