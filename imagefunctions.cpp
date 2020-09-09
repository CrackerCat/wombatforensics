#include "imagefunctions.h"

unsigned long long GetTotalBytes(std::string instr)
{
    unsigned long long totbyt = 0;
    int infile = open(instr.c_str(), O_RDONLY);
    ioctl(infile, BLKGETSIZE64, &totbyt);
    close(infile);
    return totbyt;
}

void StartImaging(std::string instr, std::string outpath, std::string outstr, int radio) 
{
    if(radio == 0)
    {
        ReadBytes(instr, std::string(outpath + "/" + outstr + ".dd"));
        Verify(instr, std::string(outpath + "/" + outstr + ".dd"));
        printf("Raw Forensic Image Finished Successfully.\n");
    }
    else if(radio == 1)
    {
        ReadBytes(instr, std::string(outpath + "/" + outstr + ".dd"));
        Verify(instr, std::string(outpath + "/" + outstr + ".dd"));
        std::string aff4cmd = std::string(getenv("HOME")) + std::string("/.local/share/wombatforensics/aff4imager");
        aff4cmd += " -i " + outpath + "/" + outstr + ".dd -i " + outpath + "/" + outstr + ".dd.log -o " + outpath + "/" + outstr + ".aff4";
        system(aff4cmd.c_str());
        std::remove(std::string(outpath + "/" + outstr + ".dd").c_str());
        printf("AFF4'd Forensic Image Finished Successfully.\n");
    }
    else if(radio == 2)
    {
        ReadBytes(instr, std::string(outpath + "/" + outstr + ".dd"));
        Verify(instr, std::string(outpath + "/" + outstr + ".dd"));
        std::string sqshcmd = "mksquashfs " + outpath + "/" + outstr + ".dd " + outpath + "/" + outstr + ".dd.log " + outpath + "/" + outstr + ".sfs";
        system(sqshcmd.c_str());
        std::remove(std::string(outpath + "/" + outstr + ".dd").c_str());
        std::remove(std::string(outpath + "/" + outstr + ".log").c_str());
        printf("Squashfs'd Forensic Image Finished Successfully.\n");
    }
}

void ReadBytes(std::string instr, std::string outstr)
{
    std::ofstream logfile;
    time_t starttime = time(NULL);
    logfile.open(outstr + ".log", std::ofstream::out | std::ofstream::app);
    char buff[35];
    logfile << "Wombat Forensics v0.3 Forensic Image started at: " << GetDateTime(buff) << "\n";
    unsigned long long totalbytes = 0;
    unsigned int sectorsize = 512;
    unsigned long long curpos = 0;
    int infile = open(instr.c_str(), O_RDONLY);
    int outfile = open(outstr.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, S_IRWXU);
    ioctl(infile, BLKGETSIZE64, &totalbytes);
    ioctl(infile, BLKSSZGET, &sectorsize);
    logfile << "Source Device: " << instr << " Size: " << totalbytes << " bytes\n";
    logfile << "Source Device: " << instr << " Block Size: " << sectorsize << " bytes\n";
    lseek(infile, 0, SEEK_SET);
    lseek(outfile, 0, SEEK_SET);
    while (curpos < totalbytes)
    {
        char bytebuf[sectorsize];
	ssize_t bytesread = read(infile, bytebuf, sectorsize);
	ssize_t byteswrite = write(outfile, bytebuf, sectorsize);
        if(bytesread == -1)
            perror("Read Error:");
	if(byteswrite == -1)
	    perror("Write Error:");
	curpos = curpos + sectorsize;
	printf("Wrote %lld out of %lld bytes\r", curpos, totalbytes);
	fflush(stdout);
    }
    time_t endtime = time(NULL);
    logfile << "Wrote " << curpos << " out of " << totalbytes << " bytes\n";
    logfile << "Forensic Image: " << outstr << "\n";
    logfile << "Forensic Image finished at: " << GetDateTime(buff) << "\n";
    logfile << "Forensic Image created in: " << difftime(endtime, starttime) << " seconds\n";
    printf("\nForensic Image Creation Finished!\n");
    logfile.close();
    close(infile);
    close(outfile);
}

std::string Verify(std::string instr)
{
    std::string inmd5 = instr;
    /*
    FILE* infile = fopen(instr.c_str(), "rb");
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    //char mess1[] = "Test Message\n";
    //char mess2[] = "Hello World\n";
    unsigned char md_value[EVP_MAX_MD_SIZE];
    int md_len, i;

    OpenSSL_add_all_digests();

    md = EVP_md5();
    //md = EVP_get_digestbyname(argv[1]); // will be md = EVP_MD5();
    // may switch to input if the user wants to calculate md5, sha1, or sha256


    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, md, NULL);
    while((bytes = fread(data, 1, 1024, instr.c_str())) != 0)
    {
        EVP_DigestUpdate(mdctx, data, bytes);
    }
    //EVP_DigestUpdate(mdctx, mess1, strlen(mess1));
    //EVP_DigestUpdate(mdctx, mess2, strlen(mess2));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_destroy(mdctx);

    printf("Digest is: ");
    for(i = 0; i < md_len; i++)
        printf("%02x", md_value[i]);
    printf("\n");

    // Call this once before exit.
    EVP_cleanup();
    */
    return inmd5;
}

void Verify(std::string instr, std::string outstr)
{
    std::ofstream logfile;
    time_t starttime = time(NULL);
    char buff[35];
    logfile.open(outstr + ".log", std::ofstream::out | std::ofstream::app);
    logfile << "Starting Image Verification at " << GetDateTime(buff) << "\n";
    unsigned long long totalbytes = 0;
    unsigned int sectorsize = 512;
    int infile1 = open(instr.c_str(), O_RDONLY);
    ioctl(infile1, BLKGETSIZE64, &totalbytes);
    ioctl(infile1, BLKSSZGET, &sectorsize);
    close(infile1);
    printf("\nStarting Image Verification...\n");
    unsigned char c[MD5_DIGEST_LENGTH];
    unsigned char o[MD5_DIGEST_LENGTH];
    int i;
    FILE* infile = fopen(instr.c_str(), "rb");
    FILE* outfile = fopen(outstr.c_str(), "rb");
    MD5_CTX mdcontext;
    MD5_CTX outcontext;
    int bytes;
    int obytes;
    unsigned char data[sectorsize];
    unsigned char odata[sectorsize];
    MD5_Init(&mdcontext);
    MD5_Init(&outcontext);
    unsigned long long curpos = 0;
    while((bytes = fread(data, 1, sectorsize, infile)) != 0)
    {
        curpos = curpos + sectorsize;
	MD5_Update(&mdcontext, data, bytes);
        obytes = fread(odata, 1, sectorsize, outfile);
        MD5_Update(&outcontext, odata, obytes);
        printf("Bytes Read: %lld/%lld\r", curpos, totalbytes);
        fflush(stdout);
    }
    logfile << "Bytes Read: " << curpos << "/" << totalbytes << "\n";
    MD5_Final(c, &mdcontext);
    MD5_Final(o, &outcontext);
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
        logfile << "Verification Successful\n";
    }
    else
    {
	printf("Verification Failed\n");
        logfile << "Verification Failed\n";
    }
    time_t endtime = time(NULL);
    logfile << "Finished Forensic Image Verification at " << GetDateTime(buff) << "\n";
    logfile << "Finished Forensic Image Verification in: " << difftime(endtime, starttime) << " seconds\n";
    logfile.close();
    fclose(infile);
    fclose(outfile);
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