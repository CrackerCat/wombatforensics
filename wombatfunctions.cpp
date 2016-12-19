#include "wombatfunctions.h"

// Copyright 2015 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

std::string GetTime()
{
    struct tm *newtime;
    time_t aclock;

    time(&aclock);   // Get time in seconds
    newtime = localtime(&aclock);   // Convert time to struct tm form 
    char timeStr[64];
    snprintf(timeStr, 64, "%.2d/%.2d/%.2d %.2d:%.2d:%.2d",
        newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year % 100, 
        newtime->tm_hour, newtime->tm_min, newtime->tm_sec);

    return timeStr;
}

void LogMessage(QString logmsg)
{
    QString tmpstring = QDateTime::currentDateTime().toString(QString("MM/dd/yyyy hh:mm:ss t"));
    //msglog->append(QString(tmpstring + " " + logmsg));
    logfile.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
    logfile.write(QString(tmpstring + "\t" + logmsg + "\n").toStdString().c_str());
    logfile.close();
}

char* TskTimeToStringUTC(time_t time, char buf[128])
{
    buf[0] = '\0';
    if (time <= 0) {
        strncpy(buf, "", 128);
    }
    else {
        //struct tm *tmTime = localtime(&time);
        struct tm *tmTime = gmtime(&time);

        snprintf(buf, 128, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", (int) tmTime->tm_year + 1900, (int) tmTime->tm_mon + 1, (int) tmTime->tm_mday, tmTime->tm_hour, (int) tmTime->tm_min, (int) tmTime->tm_sec);//, tzname[(tmTime->tm_isdst == 0) ? 0 : 1]);
    }
    return buf;
}

//unsigned long long GetChildCount(int type, unsigned long long address, unsigned long long parimgid, unsigned long long parfsid)
unsigned long long GetChildCount(QString filefilter)
{
    unsigned long long tmpcount = 0;
    QDir eviddir = QDir(wombatvariable.tmpmntpath);
    QStringList files = eviddir.entryList(QStringList(filefilter), QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    tmpcount = files.count();
    /*
    QSqlQuery childquery(fcasedb);
    QString querystring = "SELECT COUNT(id) FROM data WHERE parid = ?";
    if(type < 4)
        querystring += " AND objtype < 5";
    else
    {
        querystring += " AND objtype >= 5 AND parfsid = ?";
    }
    if(type != 1)
        querystring += " AND parimgid = ?";
    childquery.prepare(querystring);
    childquery.addBindValue(address);
    if(type >= 4)
        childquery.addBindValue(parfsid);
    if(type != 1)
        childquery.addBindValue(parimgid);
    if(childquery.exec())
    {
        childquery.next();
        tmpcount = childquery.value(0).toULongLong();
    }
    childquery.finish();
    */
    return tmpcount;
}

QStringList GetChildFiles(QString filefilter)
{
    QDir eviddir = QDir(wombatvariable.tmpmntpath);
    return eviddir.entryList(QStringList(filefilter), QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
}

bool FileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

bool ProcessingComplete()
{
    if(filesfound == filesprocessed && filesfound > 0)
        return true;
    
    return false;
}
/*
void ProcessFile(QVector<QString> tmpstrings, QVector<unsigned long long> tmpints, FileData adsdata, bool adsbool)
{
    FileData tmpdata;
    tmpdata.type = tmpints[0];
    tmpdata.name = tmpstrings[0];
    tmpdata.paraddr = tmpints[1];
    tmpdata.path = tmpstrings[1];
    tmpdata.atime = tmpints[2];
    tmpdata.ctime = tmpints[3];
    tmpdata.crtime = tmpints[4];
    tmpdata.mtime = tmpints[5];
    tmpdata.size = tmpints[6];
    tmpdata.addr = tmpints[7];
    tmpdata.evid = currentevidenceid;
    tmpdata.fsid = tmpints[8];
    tmpdata.mftattrid = 0;
    //mutex.lock();
    filedatavector.append(tmpdata);
    if(adsbool == true)
    {
        filedatavector.append(adsdata);
    }
    //mutex.unlock();
}
*/
TSK_WALK_RET_ENUM GetBlockAddress(TSK_FS_FILE* tmpfile, TSK_OFF_T off, TSK_DADDR_T addr, char* buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, void *ptr)
{
    if(off < 0)
    {
        // remove compile warning
    }
    if(buf)
    {
        // remove compile warning
    }
    if(ptr)
    {
        // remove compile warning
    }

    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_HFS_DETECT)
    {
        blockstring += QString::number(addr) + "|";
    }
    else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_ISO9660_DETECT)
    {
        // iso is already done in the previous function
    }
    else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_FAT_DETECT || tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
    {
        blockstring += QString::number(addr) + "|";
    }
    else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_YAFFS2_DETECT)
    {
        if(flags & TSK_FS_BLOCK_FLAG_CONT)
        {
            blockstring += QString::number(addr) + "|";
        }
    }
    else if(tmpfile->name != NULL)
    {
        if((strcmp(tmpfile->name->name, "$FAT1") == 0) || (strcmp(tmpfile->name->name, "$FAT2") == 0) || (strcmp(tmpfile->name->name, "$MBR") == 0) || (strcmp(tmpfile->name->name, "$OprhanFiles") == 0))
        {
            blockstring += QString::number(addr) + "|";
        }
    }
    else
    {
        if(flags & TSK_FS_BLOCK_FLAG_CONT)
        {
            int i, s;
            for(i = 0, s = (int) size; s > 0; s -= tmpfile->fs_info->block_size, i++)
            {
                if(addr)
                {
                    blockstring += QString::number(addr + i) + "|";
                }
            }
        }
    }
    return TSK_WALK_CONT;
}

QString GetFilePermissions(TSK_FS_META* tmpmeta)
{
    QString tmpstring = "----------";
    tmpstring.replace(0, 1, tsk_fs_meta_type_str[tmpmeta->type][0]);
    if(tmpmeta->mode & TSK_FS_META_MODE_IRUSR)
        tmpstring.replace(1, 1, "r");
    if(tmpmeta->mode & TSK_FS_META_MODE_IWUSR)
        tmpstring.replace(2, 1, "w");
    if(tmpmeta->mode & TSK_FS_META_MODE_ISUID)
    {
        if(tmpmeta->mode & TSK_FS_META_MODE_IXUSR)
            tmpstring.replace(3, 1, "s");
        else
            tmpstring.replace(3, 1, "S");
    }
    else if(tmpmeta->mode & TSK_FS_META_MODE_IXUSR)
        tmpstring.replace(3, 1, "x");
    if(tmpmeta->mode & TSK_FS_META_MODE_IRGRP)
        tmpstring.replace(4, 1, "r");
    if(tmpmeta->mode & TSK_FS_META_MODE_IWGRP)
        tmpstring.replace(5, 1, "w");
    if(tmpmeta->mode && TSK_FS_META_MODE_ISGID)
    {
        if(tmpmeta->mode & TSK_FS_META_MODE_IXGRP)
            tmpstring.replace(6, 1, "s");
        else
            tmpstring.replace(6, 1, "S");
    }
    else if(tmpmeta->mode & TSK_FS_META_MODE_IXGRP)
        tmpstring.replace(6, 1, "x");
    if(tmpmeta->mode & TSK_FS_META_MODE_IROTH)
        tmpstring.replace(7, 1, "r");
    if(tmpmeta->mode & TSK_FS_META_MODE_IWOTH)
        tmpstring.replace(8, 1, "w");
    if(tmpmeta->mode & TSK_FS_META_MODE_ISVTX) // sticky bit
    {
        if(tmpmeta->mode & TSK_FS_META_MODE_IXOTH)
            tmpstring.replace(9, 1, "t");
        else
            tmpstring.replace(9, 1, "T");
    }
    else if(tmpmeta->mode & TSK_FS_META_MODE_IXOTH)
        tmpstring.replace(9, 1, "x");
    return tmpstring;
}

void FileMap(FileData &filedata)
{
    /*
    QFile filefile(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".f" + QString::number(filedata.addr));
    filefile.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&filefile);
    out << filedata.name << "," << filedata.path << "," << filedata.type << "," << filedata.paraddr << "," << filedata.atime << "," << filedata.ctime << "," << filedata.crtime << "," << filedata.mtime << "," << filedata.size << "," << filedata.addr << "," << filedata.evid << "," << filedata.fsid << "," << filedata.mftattrid << "," << filedata.mimetype;
    filefile.close();
    isignals->ProgUpd();
    //emit isignals->FinishSql();
    */

}

TSK_WALK_RET_ENUM FileEntries(TSK_FS_FILE* tmpfile, const char* tmppath, void* tmpptr)
{
    QString outstring = "";
    /*
    //QFile filefile;
    if(tmpfile->meta != NULL)
    {
        qDebug() << "file addr: " << tmpfile->meta->addr;
        //filefile.setFileName(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".f" + QString::number(tmpfile->meta->addr));
    }
    else
    {
        qDebug() << "file addr: meta is null: " << tmpfile->name->meta_addr;
        //filefile.setFileName(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".f" + QString::number(tmpfile->name->meta_addr));
    }
    //filefile.open(QIODevice::Append | QIODevice::Text);
    //QTextStream out(&filefile);
    */



/* BASE 64 ENCODE/DECODE FOR THE FILE NAME TO AVOID COMMA'S IN THE FILE NAME.
 *
 *QString base64_encode(QString string){
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}
 
QString base64_decode(QString string){
    QByteArray ba;
    ba.append(string);
    return QByteArray::fromBase64(ba);
}
 * =
 *
 *
 *
 */ 



    if(tmpfile->name != NULL)
    {
        QByteArray ba;
        ba.append(QString(tmpfile->name->name));
        outstring += ba.toBase64() + "," + QString::number(tmpfile->name->type) + "," + QString::number(tmpfile->name->par_addr) + ",";
        //outstring += QString(tmpfile->name->name) + "," + QString::number(tmpfile->name->type) + "," + QString::number(tmpfile->name->par_addr) + ",";
        //out << tmpfile->name->name << "," << tmpfile->name->type << "," << tmpfile->name->par_addr << ",";
    }
    else
    {
        outstring += "unknown," + QString::number(tmpfile->meta->type) + ",0,";
        //out << "unknown," + tmpfile->meta->type + ",0,";
    }
    outstring += "/" + QString(tmppath) + ",";
    //out << "/" << QString(tmppath) << ",";
    if(tmpfile->meta != NULL)
    {
        outstring += QString::number(tmpfile->meta->atime) + "," + QString::number(tmpfile->meta->ctime) + "," + QString::number(tmpfile->meta->crtime) + "," + QString::number(tmpfile->meta->mtime) + "," + QString::number(tmpfile->meta->size) + "," + QString::number(tmpfile->meta->addr) + ",";
        //out << tmpfile->meta->atime << "," << tmpfile->meta->ctime << "," << tmpfile->meta->crtime << "," << tmpfile->meta->mtime << "," << tmpfile->meta->size << "," << tmpfile->meta->addr;
    }
    else
    {
        outstring += "0,0,0,0,0," + QString::number(tmpfile->name->meta_addr) + ",";
        //out << "0,0,0,0,0," + tmpfile->name->meta_addr + ",";
    }
    char magicbuffer[1024];
    tsk_fs_file_read(tmpfile, 0, magicbuffer, 1024, TSK_FS_FILE_READ_FLAG_NONE);
    QMimeDatabase mimedb;
    QMimeType mimetype = mimedb.mimeTypeForData(QByteArray((char*)magicbuffer));
    outstring += mimetype.name() + ",0,e" + QString::number(evidcnt) + "-v" + QString::number(volcnt) + "-p" + QString::number(partint) + "-f" + QString::number(tmpfile->name->meta_addr) + "-a" + QString::number(tmpfile->name->par_addr);

    TSK_FS_HASH_RESULTS hashresults;
    uint8_t retval = tsk_fs_file_hash_calc(tmpfile, &hashresults, TSK_BASE_HASH_MD5);
    if(retval == 0)
    {
        char sbuf[17];
        int sint = 0;
        for(int i=0; i < 16; i++)
        {
            sint = sprintf(sbuf+(2*i), "%02X", hashresults.md5_digest[i]);
        }
        outstring +=  "," + QString(sbuf);
    }
    else
        outstring += ",0";



    //out << mimetype.name() << ",0";
    QFile filefile;
    QTextStream out(&filefile);
    if(tmpfile->name != NULL)
    {
        if(strcmp(tmpfile->name->name, ".") != 0)
        {
            if(strcmp(tmpfile->name->name, "..") != 0)
            {
                    filefile.setFileName(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".p" + QString::number(partint) + ".f" + QString::number(tmpfile->name->meta_addr) + ".a" + QString::number(tmpfile->name->par_addr));
                    filefile.open(QIODevice::Append | QIODevice::Text);
                    out << outstring;
                    filesfound++;
            }
        }
    }
    else
    {
        filefile.setFileName(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".p" + QString::number(partint) + ".f" + QString::number(tmpfile->meta->addr) + ".a" + QString::number(tmpfile->name->par_addr));
        filefile.open(QIODevice::Append | QIODevice::Text);
        out << outstring;
        out.flush();
        filesfound++;
    }
    filefile.close();
    isignals->ProgUpd();

    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
    {
        QByteArray adsba;
        int tmpattrid = 0;
        unsigned long long adssize = 0;
        TSK_OFF_T curmftentrystart = 0;
        NTFS_INFO* ntfsinfo = (NTFS_INFO*)tmpfile->fs_info;
        int recordsize = 0;
        if(ntfsinfo->fs->mft_rsize_c > 0)
        {
            recordsize = ntfsinfo->fs->mft_rsize_c * ntfsinfo->fs->csize * tsk_getu16(tmpfile->fs_info->endian, ntfsinfo->fs->ssize);
        }
        else
            recordsize = 1 << -ntfsinfo->fs->mft_rsize_c;
        if(tmpfile->meta != NULL)
            curmftentrystart = tsk_getu16(tmpfile->fs_info->endian, ntfsinfo->fs->ssize) * ntfsinfo->fs->csize * tsk_getu64(tmpfile->fs_info->endian, ntfsinfo->fs->mft_clust) + recordsize * tmpfile->meta->addr + 20;
        else
            curmftentrystart = tsk_getu16(tmpfile->fs_info->endian, ntfsinfo->fs->ssize) * ntfsinfo->fs->csize * tsk_getu64(tmpfile->fs_info->endian, ntfsinfo->fs->mft_clust) + recordsize + 20;
        char startoffset[2];
        tsk_fs_read(tmpfile->fs_info, curmftentrystart, startoffset, 2);
        uint16_t teststart = startoffset[1] * 256 + startoffset[0];
        adssize = (unsigned long long)teststart;
        if(strcmp(tmpfile->name->name, ".") != 0)
        {
            if(strcmp(tmpfile->name->name, "..") != 0)
            {
                int cnt, i;
                cnt = tsk_fs_file_attr_getsize(tmpfile);
                for(i = 0; i < cnt; i++)
                {
                    char type[512];
                    const TSK_FS_ATTR* fsattr = tsk_fs_file_attr_get_idx(tmpfile, i);
                    adssize += 24;
                    adssize += (unsigned long long)fsattr->size;
                    if(ntfs_attrname_lookup(tmpfile->fs_info, fsattr->type, type, 512) == 0)
                    {
                        if(QString::compare(QString(type), "$DATA", Qt::CaseSensitive) == 0)
                        {
                            if(QString::compare(QString(fsattr->name), "") != 0 && QString::compare(QString(fsattr->name), "$I30", Qt::CaseSensitive) != 0)
                            {
                                QFile adsfile(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".p" + QString::number(partint) + ".f" + QString::number(adssize - fsattr->size + 16) + ".a" + QString::number(tmpfile->name->meta_addr));
                                //QFile adsfile(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".p" + QString::number(partint) + ".f" + QString::number(fsattr->id) + ".a" + QString::number(tmpfile->name->meta_addr));
                                //QFile adsfile(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".p" + QString::number(partint) + ".f" + QString::number(tmpfile->meta->addr) + ".f" + QString::number(adssize - (unsigned long long)fsattr->size + 16));
                                adsfile.open(QIODevice::Append | QIODevice::Text);
                                QTextStream adsout(&adsfile);
                                adsba.append(QString(tmpfile->name->name) + QString(":") + QString(fsattr->name));
                                //adsout << adsba.toBase64() << "," << tmpfile->name->type << "," << tmpfile->name->par_addr << "," << QString("/") + QString(tmppath) << ",0, 0, 0, 0," << fsattr->size << "," << adssize - (unsigned long long)fsattr->size + 16 << "," << mimetype.name() << "," << fsattr->id << ",e" + QString::number(evidcnt) + "-v" + QString::number(volcnt) + "-p" + QString::number(partint) + "-f" + QString::number(fsattr->id) << ",0";
                                adsout << adsba.toBase64() << "," << tmpfile->name->type << "," << tmpfile->meta->addr << "," << QString("/") + QString(tmppath) << ",0, 0, 0, 0," << fsattr->size << "," << adssize - (unsigned long long)fsattr->size + 16 << "," << mimetype.name() << "," << fsattr->id << ",e" + QString::number(evidcnt) + "-v" + QString::number(volcnt) + "-p" + QString::number(partint) + "-f" + QString::number(adssize - (unsigned long long)fsattr->size + 16) + "-a" + QString::number(tmpfile->name->meta_addr) << ",0";
                                adsout.flush();
                                adsfile.close();
                            }
                        }
                    }
                }
            }
        }
    }

    /*
    FileData tmpdata;
    filesfound++;
    isignals->ProgUpd();
    if(tmpptr != NULL)
    {
        //LogMessage("TmpPtr got a value somehow");
    }

    if(tmpfile->name != NULL) tmpdata.name = QString(tmpfile->name->name);
    else tmpdata.name = QString("unknown.dat");
    tmpdata.path = QString("/") + QString(tmppath);

    if(tmpfile->name != NULL)
    {
        tmpdata.type = (unsigned long long)tmpfile->name->type;
        tmpdata.paraddr = (unsigned long long)tmpfile->name->par_addr;
    }
    else
    {
        tmpdata.type = 0;
        tmpdata.type = 0;
    }
    if(tmpfile->meta != NULL)
    {
        tmpdata.atime = (unsigned long long)tmpfile->meta->atime;
        tmpdata.ctime = (unsigned long long)tmpfile->meta->ctime;
        tmpdata.crtime = (unsigned long long)tmpfile->meta->crtime;
        tmpdata.mtime = (unsigned long long)tmpfile->meta->mtime;
        tmpdata.size = (unsigned long long)tmpfile->meta->size;
        tmpdata.addr = (unsigned long long)tmpfile->meta->addr;
    }
    else
    {
        tmpdata.atime = 0;
        tmpdata.ctime = 0;
        tmpdata.crtime = 0;
        tmpdata.mtime = 0;
        tmpdata.size = 0;
        tmpdata.addr = 0;
    }
    tmpdata.evid = currentevidenceid;
    tmpdata.fsid = currentfilesystemid;
    tmpdata.mftattrid = 0;
    char magicbuffer[1024];
    tsk_fs_file_read(tmpfile, 0, magicbuffer, 1024, TSK_FS_FILE_READ_FLAG_NONE);
    QMimeDatabase mimedb;
    QMimeType mimetype = mimedb.mimeTypeForData(QByteArray((char*)magicbuffer));
    tmpdata.mimetype = mimetype.name();
    */
    /*
     *    readfileinfo = tsk_fs_file_open_meta(readfsinfo, NULL, secprocobj.address);
    char magicbuffer[1024];
    tsk_fs_file_read(readfileinfo, 0, magicbuffer, 1024, TSK_FS_FILE_READ_FLAG_NONE);


    // Begin Mime Type Determination
    QMimeDatabase mimedb;
    QMimeType mimetype = mimedb.mimeTypeForData(QByteArray((char*)magicbuffer));
    secprocobj.mimetype = mimetype.name();

     *
     */ 
    /*
    filedatavector.append(tmpdata);

    FileData adsdata;
    unsigned long long adssize = 0;
    TSK_OFF_T curmftentrystart = 0;
    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
    {
        NTFS_INFO* ntfsinfo = (NTFS_INFO*)tmpfile->fs_info;
        int recordsize = 0;
        if(ntfsinfo->fs->mft_rsize_c > 0)
        {
            recordsize = ntfsinfo->fs->mft_rsize_c * ntfsinfo->fs->csize * tsk_getu16(tmpfile->fs_info->endian, ntfsinfo->fs->ssize);
        }
        else
            recordsize = 1 << -ntfsinfo->fs->mft_rsize_c;
        if(tmpfile->meta != NULL)
            curmftentrystart = tsk_getu16(tmpfile->fs_info->endian, ntfsinfo->fs->ssize) * ntfsinfo->fs->csize * tsk_getu64(tmpfile->fs_info->endian, ntfsinfo->fs->mft_clust) + recordsize * tmpfile->meta->addr + 20;
        else
            curmftentrystart = tsk_getu16(tmpfile->fs_info->endian, ntfsinfo->fs->ssize) * ntfsinfo->fs->csize * tsk_getu64(tmpfile->fs_info->endian, ntfsinfo->fs->mft_clust) + recordsize + 20;
        char startoffset[2];
        tsk_fs_read(tmpfile->fs_info, curmftentrystart, startoffset, 2);
        uint16_t teststart = startoffset[1] * 256 + startoffset[0];
        adssize = (unsigned long long)teststart;
    }
    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
    {
        if(strcmp(tmpfile->name->name, ".") != 0)
        {
            if(strcmp(tmpfile->name->name, "..") != 0)
            {
                int cnt, i;
                cnt = tsk_fs_file_attr_getsize(tmpfile);
                for(i = 0; i < cnt; i++)
                {
                    char type[512];
                    const TSK_FS_ATTR* fsattr = tsk_fs_file_attr_get_idx(tmpfile, i);
                    adssize += 24;
                    adssize += (unsigned long long)fsattr->size;
                    if(ntfs_attrname_lookup(tmpfile->fs_info, fsattr->type, type, 512) == 0)
                    {
                        if(QString::compare(QString(type), "$DATA", Qt::CaseSensitive) == 0)
                        {
                            if(QString::compare(QString(fsattr->name), "") != 0 && QString::compare(QString(fsattr->name), "$I30", Qt::CaseSensitive) != 0)
                            {
                                adsdata.type = (unsigned long long)tmpfile->name->type;
                                adsdata.paraddr = (unsigned long long)tmpfile->meta->addr;
                                adsdata.name = QString(":") + QString(fsattr->name);
                                adsdata.path = QString("/") + QString(tmppath);
                                adsdata.atime = 0;
                                adsdata.ctime = 0;
                                adsdata.crtime = 0;
                                adsdata.mtime = 0;
                                adsdata.evid = currentevidenceid;
                                adsdata.fsid = currentfilesystemid;
                                adsdata.size = (unsigned long long)fsattr->size;
                                adsdata.addr = adssize - (unsigned long long)fsattr->size + 16;
                                adsdata.mimetype = mimetype.name();
                                adsdata.mftattrid = (unsigned long long)fsattr->id; // STORE attr id in this variable in the db.
                                filedatavector.append(adsdata);
                            }
                        }
                    }
                }
            }
        }
    }*/
    return TSK_WALK_CONT;
}

void GenerateThumbnails()
{
    /*
    QSqlQuery filequery(fcasedb);
    filequery.prepare("SELECT objectid, parimgid, parfsid, address FROM data WHERE objecttype = 5 AND filemime LIKE '%image/%';");
    if(filequery.exec())
    {
        while(filequery.next())
        {
            const TSK_TCHAR** imagepartspath;
            unsigned long long objectid = 0;
            objectid = filequery.value(0).toULongLong();
            /
            QSqlQuery thumbquery(thumbdb);
            thumbquery.prepare("SELECT COUNT(thumbid) FROM thumbs WHERE objectid = ?;");
            thumbquery.addBindValue(objectid);
            thumbquery.exec();
            thumbquery.next();
            if(thumbquery.value(0).toInt() == 0)
            {
                TSK_IMG_INFO* readimginfo;
                TSK_FS_INFO* readfsinfo;
                TSK_FS_FILE* readfileinfo;
                // Open Parent Image
                std::vector<std::string> pathvector;
                pathvector.clear();
                QSqlQuery imgquery(fcasedb);
                imgquery.prepare("SELECT fullpath FROM dataruns WHERE objectid = ? ORDER BY seqnum;");
                imgquery.bindValue(0, filequery.value(1).toULongLong());
                if(imgquery.exec())
                {
                    while(imgquery.next())
                    {
                        pathvector.push_back(imgquery.value(0).toString().toStdString());
                    }
                }
                imgquery.finish();
                imagepartspath = (const char**)malloc(pathvector.size()*sizeof(char*));
                for(uint i=0; i < pathvector.size(); i++)
                {
                    imagepartspath[i] = pathvector.at(i).c_str();
                }
                readimginfo = tsk_img_open(pathvector.size(), imagepartspath, TSK_IMG_TYPE_DETECT, 0);
                free(imagepartspath);
                //OpenParentFileSystem
                QSqlQuery fsquery(fcasedb);
                fsquery.prepare("SELECT byteoffset FROM data where objectid = ?;");
                fsquery.bindValue(0, filequery.value(2).toULongLong());
                fsquery.exec();
                fsquery.next();
                readfsinfo = tsk_fs_open_img(readimginfo, fsquery.value(0).toULongLong(), TSK_FS_TYPE_DETECT);
                fsquery.finish();
                //OpenFile
                readfileinfo = tsk_fs_file_open_meta(readfsinfo, NULL, filequery.value(3).toULongLong());
                ThumbFile(readfileinfo, objectid);    
                tsk_fs_file_close(readfileinfo);
                tsk_fs_close(readfsinfo);
                tsk_img_close(readimginfo);
            }
            thumbquery.finish();
        }
    }
    filequery.finish();
    */
}

void PropertyFile(TSK_FS_FILE* tmpfile, unsigned long long objid, unsigned long long fsoffset, int blksize, unsigned long long parfsid)
{
    QStringList proplist;
    proplist.clear();
    if(tmpfile->name != NULL) proplist << "Short Name" << tmpfile->name->shrt_name << "Short Name for a file";
    if(tmpfile->meta != NULL)
    {
        proplist << "File Permissions" << GetFilePermissions(tmpfile->meta) << "Unix Style Permissions. r - file, d - directory, l - symbolic link, c - character device, b - block device, p - named pipe, v - virtual file created by the forensic tool; r - read, w - write, x - execute, s - set id and executable, S - set id, t - sticky bit executable, T - sticky bit. format is type|user|group|other - [rdlcbpv]|rw[sSx]|rw[sSx]|rw[tTx]";
        proplist << "User ID" << QString::number(tmpfile->meta->uid) << "User ID";
        proplist << "Group ID" << QString::number(tmpfile->meta->gid) << "Group ID";
        proplist << "Allocation Status";
        if(tmpfile->meta->flags == TSK_FS_META_FLAG_ALLOC)
            proplist << "Currently Allocated";
        else if(tmpfile->meta->flags == TSK_FS_META_FLAG_UNALLOC)
            proplist << "Currently Unallocated";
        else if(tmpfile->meta->flags == TSK_FS_META_FLAG_USED)
            proplist << "Allocated at least once";
        else if(tmpfile->meta->flags == TSK_FS_META_FLAG_UNUSED)
            proplist << "Never allocated";
        else if(tmpfile->meta->flags == TSK_FS_META_FLAG_COMP)
            proplist << "Contents are compressed";
        else
            proplist << "Unspecified";
        proplist << "allocation status for the file.";

        /*
        QSqlQuery objquery(fcasedb);
        objquery.prepare("SELECT blockaddress, filemime, filesignature, address FROM data WHERE objectid = ?;");
        objquery.bindValue(0, objid);
        objquery.exec();
        objquery.next();
        proplist << "Block Address" << objquery.value(0).toString() << "List of block addresses which contain the contents of the file";
        unsigned long long tmpoffset = 0;
        unsigned long long resoffset = 0;
        unsigned long long fileaddress = 0;
        fileaddress = objquery.value(3).toULongLong();
        proplist << "Byte Offset";
        if(objquery.value(0).toString().compare("") != 0)
            tmpoffset = objquery.value(0).toString().split("|", QString::SkipEmptyParts).at(0).toULongLong()*blksize + fsoffset;
        else
        {
            if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
            {
                QSqlQuery resquery(fcasedb);
                QStringList inputs;
                QList<unsigned long long> outputs;
                inputs << "%0x0B%" << "%0x0D%" << "%0x30%" << "%0x40%";
                for(int i=0; i < inputs.count(); i++)
                {
                    resquery.prepare("SELECT value from properties where objectid = ? and description like(?);");
                    resquery.addBindValue(parfsid);
                    resquery.addBindValue(inputs.at(i));
                    resquery.exec();
                    resquery.next();
                    outputs.append(resquery.value(0).toULongLong());
                }
                resquery.finish();
                mftrecordsize = outputs.at(3);
                resoffset = ((outputs.at(0) * outputs.at(1) * outputs.at(2)) + (outputs.at(3)*fileaddress));
                tmpoffset = resoffset + fsoffset;
            }
            else
                tmpoffset = fsoffset;
        }
        proplist << QString::number(tmpoffset) << "Byte offset for the start of the file in a block or in the MFT";
        proplist << "File Signature" << objquery.value(1).toString() << objquery.value(2).toString();
        objquery.finish();

        fcasedb.transaction();
        QSqlQuery propquery(fcasedb);
        propquery.prepare("INSERT INTO properties (objectid, name, value, description) VALUES(?, ?, ?, ?);");
        for(int i=0; i < proplist.count()/3; i++)
        {
            propquery.bindValue(0, objid);
            propquery.bindValue(1, proplist.at(3*i));
            propquery.bindValue(2, proplist.at(3*i+1));
            propquery.bindValue(3, proplist.at(3*i+2));
            propquery.exec();
        }
        fcasedb.commit();
        propquery.finish();
        */
        processphase++;
        isignals->ProgUpd();
    }
}

void AlternateDataStreamPropertyFile(TSK_FS_FILE* tmpfile, QVector<unsigned long long> adsobjid, QVector<unsigned long long> adsattrid)
{
    QStringList proplist;
    proplist.clear();
    if(tmpfile->name != NULL)
    {
        QString adsinfo = "Alternate Data Stream for " + QString::fromStdString(std::string(tmpfile->name->name));
        proplist << "Alternate Data Stream (ADS)" << adsinfo << "Alternate data stream which contains different content from what the file's standard content is.";
    }
    for(int i = 0; i < adsobjid.count(); i++)
    {
        /*
        QSqlQuery adsquery(fcasedb);
        adsquery.prepare("SELECT name, parentid, blockaddress FROM data WHERE objectid = ?;");
        adsquery.bindValue(0, adsobjid.at(i));
        adsquery.exec();
        adsquery.next();
        proplist << "Name" << adsquery.value(0).toString() << "Name for the NTFS parent file additional $Data attribute";
        proplist << "Parent Address" << adsquery.value(1).toString() << "NTFS address ID for the parent file";
        if(tmpfile->name != NULL)
            proplist << "Parent File Name" << QString(tmpfile->name->name) << "File name of the parent file";
        proplist << "Block Address" << adsquery.value(2).toString() << "List of block addresses which contain the contents of the ADS";
        proplist << "Attribute ID" << QString::number(adsattrid.at(i)) << "ID for the file's ADS attribute";
        adsquery.finish();
        fcasedb.transaction();
        QSqlQuery propquery(fcasedb);
        propquery.prepare("INSERT INTO properties (objectid, name, value, description) VALUES(?, ?, ?, ?);");
        for(int j = 0; j < proplist.count()/3; j++)
        {
            propquery.bindValue(0, adsobjid.at(i));
            propquery.bindValue(1, proplist.at(3*j));
            propquery.bindValue(2, proplist.at(3*j+1));
            propquery.bindValue(3, proplist.at(3*j+2));
            propquery.exec();
        }
        fcasedb.commit();
        propquery.finish();
        */
   }
}
/*
void BlockFile(TSK_FS_FILE* tmpfile, unsigned long long objid, QVector<unsigned long long> adsattrid)
{
    blockstring = "";
    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_HFS_DETECT || tmpfile->fs_info->ftype == TSK_FS_TYPE_ISO9660_DETECT || tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT || tmpfile->fs_info->ftype == TSK_FS_TYPE_FAT_DETECT)
    {
        if(tmpfile->fs_info->ftype == TSK_FS_TYPE_HFS_DETECT)
        {
            tsk_fs_file_walk_type(tmpfile, TSK_FS_ATTR_TYPE_HFS_DATA, HFS_FS_ATTR_ID_DATA, (TSK_FS_FILE_WALK_FLAG_ENUM)(TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK), GetBlockAddress, NULL);
        }
        else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_ISO9660_DETECT)
        {
            iso9660_inode* dinode;
            dinode = (iso9660_inode*)tsk_malloc(sizeof(iso9660_inode));
            iso9660_inode_node* n;
            n = ((ISO_INFO*)tmpfile->fs_info)->in_list;
            while(n && (n->inum != tmpfile->meta->addr))
                n = n->next;
            if(n)
                memcpy(dinode, &n->inode, sizeof(iso9660_inode));
            int block = tsk_getu32(tmpfile->fs_info->endian, dinode->dr.ext_loc_m);
            TSK_OFF_T size = tmpfile->meta->size;
            while((int64_t)size > 0)
            {
                blockstring += QString::number(block++) + "|";
                size -= tmpfile->fs_info->block_size;
            }
        }
        else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
        {
            unsigned long long minads = 1000;
            for(int i = 0; i < adsattrid.count(); i++)
            {
                if(adsattrid.at(i) < minads)
                    minads = adsattrid.at(i);
            }
            if(tmpfile->meta != NULL)
            {
                if(tmpfile->meta->attr)
                {
                    int cnt, i;
                    cnt = tsk_fs_file_attr_getsize(tmpfile);
                    for(i = 0; i < cnt; i++)
                    {
                        const TSK_FS_ATTR* tmpattr = tsk_fs_file_attr_get_idx(tmpfile, i);
                        if(tmpattr->flags & TSK_FS_ATTR_NONRES) // non resident attribute
                        {
                            if(tmpattr->type == TSK_FS_ATTR_TYPE_NTFS_DATA && tmpattr->id < (int)minads)
                            {
                                tsk_fs_file_walk_type(tmpfile, tmpattr->type, tmpattr->id, (TSK_FS_FILE_WALK_FLAG_ENUM)(TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK), GetBlockAddress, NULL);
                            }
                        }
                    }
                }
            }
        }
        else if(tmpfile->fs_info->ftype == TSK_FS_TYPE_FAT_DETECT)
        {
            tsk_fs_file_walk(tmpfile, (TSK_FS_FILE_WALK_FLAG_ENUM)(TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK), GetBlockAddress, NULL);
        }
    }
    else
        tsk_fs_file_walk(tmpfile, TSK_FS_FILE_WALK_FLAG_AONLY, GetBlockAddress, NULL);
    QSqlQuery blockquery(fcasedb);
    blockquery.prepare("UPDATE data SET blockaddress = ? WHERE objectid = ?;");
    blockquery.bindValue(0, blockstring);
    blockquery.bindValue(1, objid);
    blockquery.exec();
    blockquery.next();
    blockquery.finish();
    processphase++;
    isignals->ProgUpd();
}

void AlternateDataStreamBlockFile(TSK_FS_FILE* tmpfile, QVector<unsigned long long> adsobjid, QVector<unsigned long long> adsattrid)
{
    blockstring = "";
    if(tmpfile->fs_info->ftype == TSK_FS_TYPE_NTFS_DETECT)
    {
        for(int j=0; j < adsattrid.count(); j++)
        {
            if(tmpfile->meta != NULL)
            {
                tsk_fs_file_walk_type(tmpfile, TSK_FS_ATTR_TYPE_NTFS_DATA, adsattrid.at(j), (TSK_FS_FILE_WALK_FLAG_ENUM)(TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK), GetBlockAddress, NULL);
            }
            QSqlQuery blockquery(fcasedb);
            blockquery.prepare("UPDATE data SET blockaddress = ? WHERE objectid = ?;");
            blockquery.bindValue(0, blockstring);
            blockquery.bindValue(1, adsobjid.at(j));
            blockquery.exec();
            blockquery.next();
            blockquery.finish();
        }
    }
}
*/
/*
QVariant MagicFile(TSK_FS_FILE* tmpfile, unsigned long long objid)
{
    QVariant tmpvariant;
    // FILE MIME TYPE
    char magicbuffer[1024];
    //const char* mimesig;
    //const char* sigtype;
    //char* sigp1;
    //char* sigp2;
    ssize_t readlen = tsk_fs_file_read(tmpfile, 0, magicbuffer, 1024, TSK_FS_FILE_READ_FLAG_NONE);
    if(readlen > 0)
    {
        mimesig = magic_buffer(magicmimeptr, magicbuffer, readlen);
        sigp1 = strtok((char*)mimesig, ";");
        sigtype = magic_buffer(magicptr, magicbuffer, readlen);
        sigp2 = strtok((char*)sigtype, ",");
    }
    QSqlQuery mimequery(fcasedb);
    mimequery.prepare("UPDATE data SET filemime = ?, filesignature = ? WHERE objectid = ?;");
    if(readlen > 0)
    {
        tmpvariant = QVariant(QString::fromStdString(sigp2));
        mimequery.bindValue(0, QString::fromStdString(sigp1));
        mimequery.bindValue(1, QString::fromStdString(sigp2));
    }
    else
    {
        tmpvariant = QVariant(QString::fromStdString("Zero File"));
        mimequery.bindValue(0, QString("Zero File"));
        mimequery.bindValue(1, QString("Zero File"));
    }
    mimequery.bindValue(2, objid);
    mimequery.exec();
    mimequery.next();
    mimequery.finish();
    processphase++;
    isignals->ProgUpd();
    return tmpvariant;
}
*/
/*
QVariant AlternateDataStreamMagicFile(TSK_FS_FILE* tmpfile, unsigned long long adsobjid)
{
    QVariant tmpvariant;
    off_t retval = 0;
    char magicbuffer[1024];
    int chunksize = 1024;
    //const char* mimesig;
    //const char* sigtype;
    //char* sigp1;
    //char* sigp2;
    QSqlQuery adsquery(fcasedb);
    adsquery.prepare("SELECT name, fullpath, parimgid, parfsid, parentid, address, mftattrid, size, blockaddress FROM data WHERE objectid = ?;");
    adsquery.bindValue(0, adsobjid);
    adsquery.exec();
    adsquery.next();
    if(adsquery.value(7).toULongLong() < (unsigned)chunksize)
        chunksize = adsquery.value(7).toULongLong();
    if(adsquery.value(8).toString().compare("") != 0)
    {
        if(adsquery.value(8).toString().split("|", QString::SkipEmptyParts).at(0).toULongLong() == 0) // block address empty
        {
            retval = tsk_fs_file_read_type(tmpfile, TSK_FS_ATTR_TYPE_NTFS_DATA, adsquery.value(6).toInt(), 0, magicbuffer, chunksize, TSK_FS_FILE_READ_FLAG_NONE);
        }
        else // block address contains valid data
        {
            retval = tsk_fs_read_block(tmpfile->fs_info, adsquery.value(8).toString().split("|", QString::SkipEmptyParts).at(0).toULongLong(), magicbuffer, chunksize);
        }
    }
    else // blockaddress = ""
    {
        if(adsquery.value(7).toULongLong() != 0)
        {
            retval = tsk_fs_file_read_type(tmpfile, TSK_FS_ATTR_TYPE_NTFS_DATA, adsquery.value(6).toInt(), 0, magicbuffer, chunksize, TSK_FS_FILE_READ_FLAG_NONE);
        }
    }
    if(retval > 0)
    {
        mimesig = magic_buffer(magicmimeptr, magicbuffer, chunksize);
        sigp1 = strtok((char*)mimesig, ";");
        sigtype = magic_buffer(magicptr, magicbuffer, chunksize);
        sigp2 = strtok((char*)sigtype, ";");
    }
    QSqlQuery mimequery(fcasedb);
    mimequery.prepare("UPDATE data SET filemime = ?, filesignature = ? WHERE objectid = ?;");
    if(retval > 0)
    {
        tmpvariant = QVariant(QString::fromStdString(sigp2));
        mimequery.bindValue(0, QString::fromStdString(sigp1));
        mimequery.bindValue(1, QString::fromStdString(sigp2));
    }
    else
    {
        tmpvariant = QVariant(QString("Zero File"));
        mimequery.bindValue(0, QString("Zero File"));
        mimequery.bindValue(1, QString("Zero File"));
    }
    mimequery.bindValue(2,  adsobjid);
    mimequery.exec();
    mimequery.next();
    mimequery.finish();
    adsquery.finish();
    return tmpvariant;
}
*/

void ThumbFile(TSK_FS_FILE* tmpfile, unsigned long long objid)
{
    /*
    QSqlQuery thumbquery(thumbdb);
    thumbquery.prepare("SELECT COUNT(thumbid) FROM thumbs WHERE objectid = ?;");
    thumbquery.addBindValue(objid);
    thumbquery.exec();
    thumbquery.next();
    if(thumbquery.value(0).toInt() == 0)
    {
        if(tmpfile->meta != NULL)
        {
            QByteArray thumbdata;
            QImage thumbimage;
            QBuffer thumbuf(&thumbdata);
            QImage origimage;
            char imagebuffer[tmpfile->meta->size];
            ssize_t imglen = tsk_fs_file_read(tmpfile, 0, imagebuffer, tmpfile->meta->size, TSK_FS_FILE_READ_FLAG_NONE);
            bool imageloaded = origimage.loadFromData(QByteArray::fromRawData(imagebuffer, imglen));
            if(imageloaded)
            {
                thumbimage = origimage.scaled(QSize(320, 320), Qt::KeepAspectRatio, Qt::FastTransformation);
                thumbuf.open(QIODevice::WriteOnly);
                thumbimage.save(&thumbuf, "PNG");
                thumbdata = thumbdata.toBase64();
                QSqlQuery imgquery(thumbdb);
                imgquery.prepare("INSERT INTO thumbs(objectid, thumbblob) VALUES(?, ?);");
                imgquery.bindValue(0, objid);
                imgquery.bindValue(1, QString(thumbdata));
                imgquery.exec();
                imgquery.next();
                imgquery.finish();
            }
        }
    }
    */
}

QVariant HashFile(TSK_FS_FILE* tmpfile, unsigned long long objid)
{
    QVariant tmpvariant;
    TSK_FS_HASH_RESULTS hashresults;
    uint8_t retval = tsk_fs_file_hash_calc(tmpfile, &hashresults, TSK_BASE_HASH_MD5);
    if(retval == 0)
    {
        char sbuf[17];
        int sint = 0;
        for(int i=0; i < 16; i++)
        {
            sint = sprintf(sbuf+(2*i), "%02X", hashresults.md5_digest[i]);
        }
        /*
        QSqlQuery hashquery(fcasedb);
        hashquery.prepare("UPDATE data SET md5 = ? where objectid = ?;");
        if(sint > 0)
        {
            hashquery.bindValue(0, QString(sbuf));
            tmpvariant = QVariant(QString(sbuf));
        }
        else
        {
            hashquery.bindValue(0, QString(""));
            tmpvariant = QVariant(QString(""));
        }
        hashquery.bindValue(1, objid);
        hashquery.exec();
        hashquery.next();
        hashquery.finish();
        */
    }
    return tmpvariant;
}

void cnid_to_array(uint32_t cnid, uint8_t array[4])
{
    array[3] = (cnid >> 0) & 0xff;
    array[2] = (cnid >> 8) & 0xff;
    array[1] = (cnid >> 16) & 0xff;
    array[0] = (cnid >> 24) & 0xff;
}

QImage MakeThumb(const QString &img)
{
    QByteArray ba = QByteArray::fromBase64(QByteArray::fromStdString(img.toStdString()));
    if(ba.length() > 0)
        return QImage::fromData(ba, "PNG").scaled(QSize(thumbsize, thumbsize), Qt::KeepAspectRatio, Qt::FastTransformation);
    else
        return QImage(":/bar/missingthumb").scaled(QSize(thumbsize, thumbsize), Qt::KeepAspectRatio, Qt::FastTransformation);
}

void SqlMap(FileData &filedata)
{
    //QMutexLocker locker(&mutex);
    /*
    QSqlQuery fquery(fcasedb);
    //wombattableschema << "CREATE TABLE data(objectid INTEGER PRIMARY KEY, objecttype INTEGER, type INTEGER, name TEXT, fullpath TEXT, address INTEGER, parentid INTEGER, parimgid INTEGER, parfsid INTEGER, ctime INTEGER, crtime INTEGER, atime INTEGER, mtime INTEGER, md5 TEXT NOT NULL DEFAULT "", filemime TEXT, known INTEGER, checked INTEGER NOT NULL DEFAULT 0, mftattrid INTEGER NOT NULL DEFAULT 0);";
    fquery.prepare("INSERT INTO data(objtype, type, name, fullpath, size, addr, parid, parimgid, parfsid, ctime, crtime, atime, mtime, filemime, mftattrid) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
    if(filedata.mftattrid == 0)
    {
        fquery.bindValue(0, 5);
        filesprocessed++;
        processphase++;
    }
    else
        fquery.bindValue(0, 6);
    fquery.bindValue(1, filedata.type);
    fquery.bindValue(2, filedata.name);
    fquery.bindValue(3, filedata.path);
    fquery.bindValue(4, filedata.size);
    fquery.bindValue(5, filedata.addr);
    fquery.bindValue(6, filedata.paraddr);
    fquery.bindValue(7, filedata.evid);
    fquery.bindValue(8, filedata.fsid);
    fquery.bindValue(9, filedata.ctime);
    fquery.bindValue(10, filedata.crtime);
    fquery.bindValue(11, filedata.atime);
    fquery.bindValue(12, filedata.mtime);
    fquery.bindValue(13, filedata.mimetype);
    fquery.bindValue(14, filedata.mftattrid);
    /*
    fquery.bindValue(4, filedata.paraddr);
    fquery.bindValue(4, filedata.path);
    fquery.bindValue(6, filedata.ctime);
    fquery.bindValue(9, filedata.size);
    fquery.bindValue(10, filedata.addr);
    //fquery.bindValue(14, filedata.mimetype.split("/").at(0));
    */
    /*
    fquery.exec();
    //fquery.next();
    isignals->ProgUpd();
    //emit isignals->FinishSql();
    fquery.finish();
    */
}
void InitializeEvidenceStructure(WombatVariable &wombatvariable)
{
    readimginfo = NULL;
    readvsinfo = NULL;
    readfsinfo = NULL;
    readfileinfo = NULL;
    //AddNewEvidence();
    const TSK_TCHAR** images;
    images = (const char**)malloc(wombatvariable.evidenceobject.fullpathvector.size()*sizeof(char*));
    for(uint i=0; i < wombatvariable.evidenceobject.fullpathvector.size(); i++)
    {
        images[i] = wombatvariable.evidenceobject.fullpathvector[i].c_str();
    }
    readimginfo = tsk_img_open(wombatvariable.evidenceobject.itemcount, images, TSK_IMG_TYPE_DETECT, 0);
    if(readimginfo == NULL)
    {
        LogMessage("Evidence Image acess failed");
        errorcount++;
    }
    free(images);
    fsobjectlist.clear();
    // PUT FILES IN THE SPARSE FILE NOW.
    QFile evidfile(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".evid." + QString::number(evidcnt));
    evidfile.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&evidfile);
    out << (int)readimginfo->itype << "," << (unsigned long long)readimginfo->size << "," << (int)readimginfo->sector_size << ",";
    for(unsigned int i=0; i < wombatvariable.evidenceobject.itemcount; i++)
    {
        if(i > 0 && i < wombatvariable.evidenceobject.itemcount - 2)
            out << "|";
        out << QString::fromStdString(wombatvariable.evidenceobject.fullpathvector[i]);
    }
    out << "," << wombatvariable.evidenceobject.itemcount << ",e" + QString::number(evidcnt);
    evidfile.close();
    // Write Evidence Properties Here...
    WriteEvidenceProperties(readimginfo);
    readvsinfo = tsk_vs_open(readimginfo, 0, TSK_VS_TYPE_DETECT);
    QString volname = "Dummy Volume";
    int voltype = 240;
    int volsectorsize = (int)readimginfo->sector_size;
    unsigned long long voloffset = 0;
    if(readvsinfo != NULL)
    {
        voltype = (int)readvsinfo->vstype;
        volname = QString::fromUtf8(tsk_vs_type_todesc(readvsinfo->vstype));
        volsectorsize = (int)readvsinfo->block_size;
        voloffset = (unsigned long long)readvsinfo->offset;
    }
    QFile volfile(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".vol");
    volfile.open(QIODevice::Append | QIODevice::Text);
    out.setDevice(&volfile);
    out << voltype << "," << (unsigned long long)readimginfo->size << "," << volname << "," << volsectorsize << "," << voloffset << ",e" + QString::number(evidcnt) + "-v" + QString::number(volcnt);
    volfile.close();
    if(readvsinfo == NULL) // No volume, so a single file system is all there is in the image.
    {
        readfsinfo = tsk_fs_open_img(readimginfo, 0, TSK_FS_TYPE_DETECT);
        QFile pfile(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".part.0");
        pfile.open(QIODevice::Append | QIODevice::Text);
        out.setDevice(&pfile);
        out << readfsinfo->ftype << "," << (unsigned long long)readfsinfo->block_size * (unsigned long long)readfsinfo->block_count << "," << GetFileSystemLabel(readfsinfo) << "," << (unsigned long long)readfsinfo->root_inum << "," << (unsigned long long)readfsinfo->offset << "," << (unsigned long long)readfsinfo->block_count << "," << (int)readfsinfo->block_size << ",0,0,0,e" << QString::number(evidcnt) + "-v" + QString::number(volcnt) + "-p" + QString::number(partint);
        pfile.close();
        uint8_t walkreturn;
        int walkflags = TSK_FS_DIR_WALK_FLAG_ALLOC | TSK_FS_DIR_WALK_FLAG_UNALLOC | TSK_FS_DIR_WALK_FLAG_RECURSE;
        walkreturn = tsk_fs_dir_walk(readfsinfo, readfsinfo->root_inum, (TSK_FS_DIR_WALK_FLAG_ENUM)walkflags, FileEntries, NULL);
        if(walkreturn == 1)
        {
            LogMessage("Issues with traversing the file structure were encountered");
            errorcount++;
        }
    }
    else
    {
        QFile pfile;
        if(readvsinfo->part_count > 0)
        {
            for(uint32_t i=0; i < readvsinfo->part_count; i++)
            {
                readpartinfo = tsk_vs_part_get(readvsinfo, i);
                pfile.setFileName(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".part." + QString::number(partint));
                pfile.open(QIODevice::Append | QIODevice::Text);
                out.setDevice(&pfile);
                //qDebug() << readpartinfo->flags;
                if(readpartinfo->flags == 0x02 || readpartinfo->flags == 0x04) // unallocated partition or meta entry
                {
                    out << "0," << (unsigned long long)readpartinfo->len * readvsinfo->block_size << "," << QString(readpartinfo->desc) << ",0," << readpartinfo->start << "," << (unsigned long long)readpartinfo->len << "," << (int)readvsinfo->block_size << "," << readpartinfo->flags << "," << (unsigned long long)readpartinfo->len << "," << (int)readvsinfo->block_size << ",e" + QString::number(evidcnt) + "-v" + QString::number(volcnt) + "-p" + QString::number(partint);
                    pfile.close();
                }
                else if(readpartinfo->flags == 0x01) // allocated partition
                {
                    readfsinfo = tsk_fs_open_vol(readpartinfo, TSK_FS_TYPE_DETECT);
                    if(readfsinfo != NULL)
                    {
                        out << readfsinfo->ftype << "," << (unsigned long long)readfsinfo->block_size * (unsigned long long)readfsinfo->block_count << "," << GetFileSystemLabel(readfsinfo) << "," << (unsigned long long)readfsinfo->root_inum << "," << (unsigned long long)readfsinfo->offset << "," << (unsigned long long)readfsinfo->block_count << "," << (int)readfsinfo->block_size << "," << readpartinfo->flags << "," << (unsigned long long)readpartinfo->len << "," << (int)readfsinfo->dev_bsize << ",e" + QString::number(evidcnt) + "-v" + QString::number(volcnt) + "-p" + QString::number(partint);
                        pfile.close();
                        uint8_t walkreturn;
                        int walkflags = TSK_FS_DIR_WALK_FLAG_ALLOC | TSK_FS_DIR_WALK_FLAG_UNALLOC | TSK_FS_DIR_WALK_FLAG_RECURSE;
                        walkreturn = tsk_fs_dir_walk(readfsinfo, readfsinfo->root_inum, (TSK_FS_DIR_WALK_FLAG_ENUM)walkflags, FileEntries, NULL);
                        if(walkreturn == 1)
                        {
                            LogMessage("Issues with traversing the file structure were encountered");
                            errorcount++;
                        }
                    }
                }
                partint++;
                //else if(readpartinfo->flags == 0x04) // meta partition
                //pfile.close();
            }
        }
    }
            // finished initial partition/file system information including file info stored into the vector 
            // place InitializeQueryModel(); in here...

}

void WriteEvidenceProperties(TSK_IMG_INFO* curimginfo)
{
    QFile epropfile(wombatvariable.tmpmntpath + wombatvariable.evidenceobject.name + ".eprop." + QString::number(evidcnt));
    epropfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream proplist(&epropfile);
    IMG_EWF_INFO* ewfinfo = NULL;
    uint8_t* ewfvalue = (uint8_t*)malloc(sizeof(uint8_t)*64);
    uint8_t uvalue8bit = 0;
    int8_t value8bit = 0;
    uint32_t value32bit = 0;
    uint64_t value64bit = 0;
    size64_t size64bit = 0;
    libewf_error_t* ewferror = NULL;
    proplist << QString("File Format,") << QString(tsk_img_type_todesc((TSK_IMG_TYPE_ENUM)curimginfo->itype)) << QString(,"File Format the evidence data is stored in. Usually it is either a raw image (.dd/.001) or an embedded image (.E01/.AFF). A raw image contains only the data from the evidence. The embedded image contains other descriptive information from the acquisition.");
    proplist << QString("Sector Size") << QString(QString::number(curimginfo->sector_size) + " bytes") << QString("Sector size of the device. A Sector is a subdivision of a disk where data is stored. It is the smallest value used to divide the disk.");
    proplist << QString("Sector Count") << QString(QString::number((int)((float)curimginfo->size/(float)curimginfo->sector_size)) + " sectors") << QString("The number of sectors in the disk.");
    proplist << QString("Image Path") << QString::fromStdString(string(wombatvariable.evidenceobject.fullpathvector[0])) << QString("Location where the evidence image is stored and read from.");
    if(TSK_IMG_TYPE_ISAFF(curimginfo->itype)) // its AFF
    {
    }
    else if(TSK_IMG_TYPE_ISEWF(curimginfo->itype)) // its EWF
    {
        ewfinfo = (IMG_EWF_INFO*)curimginfo;
        if(libewf_handle_get_utf8_header_value_case_number(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Case Number" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "The case number the image is associated";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_description(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Description" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Description of the acquisition and or evidence item";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_examiner_name(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Examiner Name" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Name of the examiner who acquired the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_evidence_number(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Evidence Number" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Unique number identifying the evidence item";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_notes(ewfinfo->handle, ewfvalue, 255, &ewferror) == 1)
            proplist << "Notes" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Any notes related to the acquisition";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_acquiry_date(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Acquisition Date" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Date the acquisition was made";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_system_date(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "System Date" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Date for the system acquiring the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_acquiry_operating_system(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Aquisition OS" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Operating System acquiring the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_acquiry_software_version(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Software Version Used" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Version of the software used to acquire the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_password(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Password" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Password to protect the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_model(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Model" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Model of the drive acquired";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_serial_number(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "Serial Number" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Serial number of the drive acquired";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_sectors_per_chunk(ewfinfo->handle, &value32bit, &ewferror) == 1)
            proplist << QString("Sectors Per Chunk") << QString::number(value32bit) << "Number of sectors in a image evidence chunk";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_format(ewfinfo->handle, &uvalue8bit, &ewferror) == 1)
        {
            proplist << QString("File Format");
            switch(uvalue8bit)
            {
                case LIBEWF_FORMAT_EWF:
                    proplist << QString("Original EWF") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_SMART:
                    proplist << QString("SMART") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_FTK:
                    proplist << QString("FTK Imager") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE1:
                    proplist << QString("EnCase 1") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE2:
                    proplist << QString("EnCase 2") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE3:
                    proplist << QString("EnCase 3") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE4:
                    proplist << QString("EnCase 4") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE5:
                    proplist << QString("EnCase 5") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE6:
                    proplist << QString("EnCase 6") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_LINEN5:
                    proplist << QString("Linen 5") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_LINEN6:
                    proplist << QString("Linen 6") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_EWFX:
                    proplist << QString("EWFX (extended ewf)") << QString("Extended EWF Format used to store the evidence image");
                    break;
                case LIBEWF_FORMAT_LOGICAL_ENCASE5:
                    proplist << QString("LEF EnCase 5") << QString("Logical Evidence File EnCase 5 Format used to store the evidence image");
                    break;
                case LIBEWF_FORMAT_LOGICAL_ENCASE6:
                    proplist << QString("LEF EnCase 6") << QString("Logical Evidence File EnCase 6 Format used to store the evidence image");
                    break;
                case LIBEWF_FORMAT_UNKNOWN:
                    proplist << QString("Unknown Format") << "Format used to store the evidence image";
                    break;
            }
        }
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_error_granularity(ewfinfo->handle, &value32bit, &ewferror) == 1)
            proplist << QString("Error Granularity") << QString::number(value32bit) << "Error block size";
        else
            libewf_error_fprint(ewferror, stdout);
        proplist << "Compression Method" << "Deflate" << "Method used to compress the image";
        if(libewf_handle_get_compression_values(ewfinfo->handle, &value8bit, &uvalue8bit, &ewferror) == 1)
        {
            proplist << "Compression Level";
            if(value8bit == LIBEWF_COMPRESSION_NONE)
                proplist << "No Compression";
            else if(value8bit == LIBEWF_COMPRESSION_FAST)
                proplist << "Good (Fast) Compression";
            else if(value8bit == LIBEWF_COMPRESSION_BEST)
                proplist << "Best Compression";
            else
                proplist << "Unknown Compression";
            proplist << "The more compression, the slower the acquisition but the smaller the file size";
        }
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_media_type(ewfinfo->handle, &uvalue8bit, &ewferror) == 1)
        {
            proplist << "Media Type";
            if(uvalue8bit == LIBEWF_MEDIA_TYPE_REMOVABLE)
                proplist << "Removable Disk";
            else if(uvalue8bit == LIBEWF_MEDIA_TYPE_FIXED)
                proplist << "Fixed Disk";
            else if(uvalue8bit == LIBEWF_MEDIA_TYPE_SINGLE_FILES)
                proplist << "Single Files";
            else if(uvalue8bit == LIBEWF_MEDIA_TYPE_OPTICAL)
                proplist << "Optical Disk (CD/DVD/BD)";
            else if(uvalue8bit == LIBEWF_MEDIA_TYPE_MEMORY)
                proplist << "Memory (RAM)";
            else
                proplist << "Unknown";
            proplist << "Media type of the original evidence";
        }
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_media_flags(ewfinfo->handle, &uvalue8bit, &ewferror) == 1)
        {
            if(uvalue8bit == LIBEWF_MEDIA_FLAG_PHYSICAL)
                proplist << "Media Flag" << "Physical" << "Directly connected disk";
            if(uvalue8bit == LIBEWF_MEDIA_FLAG_FASTBLOC)
                proplist << "Media Flag" << "Fastbloc Write Blocked" << "Write blocked disk using Fastbloc";
            if(uvalue8bit == LIBEWF_MEDIA_FLAG_TABLEAU)
                proplist << "Media Flag" << "Tableau Write Blocked" << "Write blocked disk using Tableau";
        }
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_bytes_per_sector(ewfinfo->handle, &value32bit, &ewferror) == 1)
            proplist << "Bytes Per Sector" << QString::number(value32bit) << "Number of bytes in a sector";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_number_of_sectors(ewfinfo->handle, &value64bit, &ewferror) == 1)
            proplist << "Number of Sectors" << QString::number(value64bit) << "Number of total sectors in the original media";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_chunk_size(ewfinfo->handle, &value32bit, &ewferror) == 1)
            proplist << "Chunk Size" << QString::number(value32bit) << "The size of an image chunk";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_media_size(ewfinfo->handle, &size64bit, &ewferror) == 1)
            proplist << "Media Size" << QString::number(size64bit) << "The size of the media";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_hash_value_md5(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "MD5" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "The MD5 hash algorithm of the uncompressed image stored as a 128-bit value";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_hash_value_sha1(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            proplist << "SHA1" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "The SHA1 hash algorithm of the uncompressed image stored as a 160-bit value";
        else
            libewf_error_fprint(ewferror, stdout);
        free(ewfvalue);
    }
    else if(TSK_IMG_TYPE_ISRAW(curimginfo->itype)) // is raw
    {
        // nothing i want to add for raw right now.
    }
    else // not supported...
    {
        // log error about unsupported image type.
    } 
    epropfile.close();
}

QString GetFileSystemLabel(TSK_FS_INFO* curinfo)
{
    if(curinfo != NULL)
    {
        if(curinfo->ftype == TSK_FS_TYPE_EXT2 || curinfo->ftype == TSK_FS_TYPE_EXT3 || curinfo->ftype == TSK_FS_TYPE_EXT4 || curinfo->ftype == TSK_FS_TYPE_EXT_DETECT)
        {
            return QString::fromStdString(string(((EXT2FS_INFO*)curinfo)->fs->s_volume_name));
        }
        else if(curinfo->ftype == TSK_FS_TYPE_FFS1 || curinfo->ftype == TSK_FS_TYPE_FFS1B)
        {
            return "UFS1";
        }
        else if(curinfo->ftype == TSK_FS_TYPE_FFS2 || curinfo->ftype == TSK_FS_TYPE_FFS_DETECT)
        {
            return QString::fromUtf8(reinterpret_cast<char*>(((FFS_INFO*)curinfo)->fs.sb2->volname));
        }
        else if(curinfo->ftype == TSK_FS_TYPE_FAT12 || curinfo->ftype == TSK_FS_TYPE_FAT16)
        {
            sprintf(asc, "%c%c%c%c%c%c%c%c%c%c%c", ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[0], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[1], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[2], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[3], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[4], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[5], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[6], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[7], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[8], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[9], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f16.vol_lab[10]);
            return QString::fromStdString(string(asc));
        }
        else if(curinfo->ftype == TSK_FS_TYPE_FAT32)
        {
            sprintf(asc, "%c%c%c%c%c%c%c%c%c%c%c", ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[0], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[1], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[2], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[3], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[4], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[5], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[6], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[7], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[8], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[9], ((FATXXFS_SB*)((FATFS_INFO*)curinfo)->boot_sector_buffer)->a.f32.vol_lab[10]);
            return QString::fromStdString(string(asc));
        }
        else if(curinfo->ftype == TSK_FS_TYPE_NTFS)
        {
            TSK_FS_FILE* tmpfile = NULL;
            const TSK_FS_ATTR* tmpattr;
            if((tmpfile = tsk_fs_file_open_meta(curinfo, NULL, NTFS_MFT_VOL)) != NULL)
            {
                tmpattr = tsk_fs_attrlist_get(tmpfile->meta->attr, TSK_FS_ATTR_TYPE_NTFS_VNAME);
                if((tmpattr->flags & TSK_FS_ATTR_RES) && (tmpattr->size))
                {
                    UTF16* name16 = (UTF16*) tmpattr->rd.buf;
                    UTF8* name8 = (UTF8*) asc;
                    int retval;
                    retval = tsk_UTF16toUTF8(curinfo->endian, (const UTF16**)&name16, (UTF16*) ((uintptr_t)name16 + (int) tmpattr->size), &name8, (UTF8*) ((uintptr_t)name8 + sizeof(asc)), TSKlenientConversion);
                    if(retval != TSKconversionOK)
                        *name8 = '\0';
                    else if((uintptr_t)name8 >= (uintptr_t)asc + sizeof(asc))
                        asc[sizeof(asc)-1] = '\0';
                    else
                        *name8 = '\0';
                    return QString::fromStdString(string(asc));
                }
            }
        }
        else if(curinfo->ftype == TSK_FS_TYPE_EXFAT)
        {
            char* databuffer = NULL;
            TSK_DADDR_T cursector = 0;
            TSK_DADDR_T endsector = 0;
            int8_t isallocsec = 0;
            TSK_INUM_T curinum = 0;
            FATFS_DENTRY* dentry = NULL;
            TSK_FS_FILE* tmpfile = NULL;
            ssize_t bytesread = 0;
            if((tmpfile = tsk_fs_file_alloc(curinfo)) != NULL)
            {
                if((tmpfile->meta = tsk_fs_meta_alloc(FATFS_FILE_CONTENT_LEN)) != NULL)
                {
                    if((databuffer = (char*)tsk_malloc(((FATFS_INFO*)curinfo)->ssize)) != NULL)
                    {
                        cursector = ((FATFS_INFO*)curinfo)->rootsect;
                        endsector = (((FATFS_INFO*)curinfo)->firstdatasect + ((FATFS_INFO*)curinfo)->clustcnt * ((FATFS_INFO*)curinfo)->csize) - 1;
                        while(cursector < endsector)
                        {
                        }
                        bytesread = tsk_fs_read_block(curinfo, cursector, databuffer, ((FATFS_INFO*)curinfo)->ssize);
                        if(bytesread == ((FATFS_INFO*)curinfo)->ssize)
                        {
                            isallocsec = fatfs_is_sectalloc(((FATFS_INFO*)curinfo), cursector);
                            if(isallocsec != -1)
                            {
                                curinum = FATFS_SECT_2_INODE(((FATFS_INFO*)curinfo), cursector);
                                for(int i = 0; i < ((FATFS_INFO*)curinfo)->ssize; i+= sizeof(FATFS_DENTRY))
                                {
                                    dentry = (FATFS_DENTRY*)&(databuffer[i]);
                                    if(exfatfs_get_enum_from_type(dentry->data[0]) == EXFATFS_DIR_ENTRY_TYPE_VOLUME_LABEL)
                                    {
                                        if(exfatfs_dinode_copy(((FATFS_INFO*)curinfo), curinum, dentry, isallocsec, tmpfile) == TSK_OK)
                                            return QString::fromStdString(tmpfile->meta->name2->name);
                                    }
                                }
                            }
                        }
                    }
                    tsk_fs_file_close(tmpfile);
                    free(databuffer);
                }
            }
        }
        else if(curinfo->ftype == TSK_FS_TYPE_ISO9660)
        {
            for(p = ((ISO_INFO*)curinfo)->pvd; p != NULL; p = p->next)
            {
                return QString::fromUtf8(reinterpret_cast<char*>(p->pvd.vol_id));
            }
        }
        else if(curinfo->ftype == TSK_FS_TYPE_YAFFS2)
        {
            return "YAFFS2";
        }
        else if(curinfo->ftype == TSK_FS_TYPE_SWAP)
        {
            return "SWAP";
        }
        else if(curinfo->ftype == TSK_FS_TYPE_HFS)
        {
            hfs = (HFS_INFO*)curinfo;
            char fn[HFS_MAXNAMLEN + 1];
            HFS_ENTRY entry;
            if(hfs_cat_file_lookup(hfs, HFS_ROOT_INUM, &entry, FALSE))
            {
                // log error here
            }
            if(hfs_UTF16toUTF8(curinfo, entry.thread.name.unicode, tsk_getu16(curinfo->endian, entry.thread.name.length), fn, HFS_MAXNAMLEN + 1, HFS_U16U8_FLAG_REPLACE_SLASH))
            {
                // log error here
            }
            sprintf(asc, "%s", fn);
            return QString::fromStdString(string(asc));
        }
        return "";
    }
    return "";
}

uint8_t hfs_cat_file_lookup(HFS_INFO* hfs, TSK_INUM_T inum, HFS_ENTRY* entry, unsigned char follow_hard_link)
{
    TSK_FS_INFO *fs = (TSK_FS_INFO *) & (hfs->fs_info);
    hfs_btree_key_cat key;      /* current catalog key */
    hfs_thread thread;          /* thread record */
    hfs_file_folder record;     /* file/folder record */
    TSK_OFF_T off;

    // Test if this is a special file that is not located in the catalog
    if ((inum == HFS_EXTENTS_FILE_ID) ||
        (inum == HFS_CATALOG_FILE_ID) ||
        (inum == HFS_ALLOCATION_FILE_ID) ||
        (inum == HFS_STARTUP_FILE_ID) ||
        (inum == HFS_ATTRIBUTES_FILE_ID)) {
        return 1;
    }


    /* first look up the thread record for the item we're searching for */

    /* set up the thread record key */
    memset((char *) &key, 0, sizeof(hfs_btree_key_cat));
    cnid_to_array((uint32_t) inum, key.parent_cnid);

    /* look up the thread record */
    off = hfs_cat_get_record_offset(hfs, &key);
    if (off == 0)
    {
        // put error code here...
        return 1;
    }

    /* read the thread record */
    if (hfs_cat_read_thread_record(hfs, off, &thread))
    {
        //tsk_error_set_errstr2(" hfs_cat_file_lookup: file (%" PRIuINUM ")", inum);
        return 1;
    }

    /* now look up the actual file/folder record */

    memset((char *) &key, 0, sizeof(hfs_btree_key_cat));
    memset((char *) &key, 0, sizeof(hfs_btree_key_cat));
    memcpy((char *) key.parent_cnid, (char *) thread.parent_cnid,
    sizeof(key.parent_cnid));
    memcpy((char *) &key.name, (char *) &thread.name, sizeof(key.name));

    /* look up the record */
    off = hfs_cat_get_record_offset(hfs, &key);
    if (off == 0)
    {
        // print error here
        return 1;
    }

    /* read the record */
    if (hfs_cat_read_file_folder_record(hfs, off, &record))
    {
        //tsk_error_set_errstr2(" hfs_cat_file_lookup: file (%" PRIuINUM ")", inum);
        return 1;
    }

    /* these memcpy can be gotten rid of, really */
    if (tsk_getu16(fs->endian, record.file.std.rec_type) == HFS_FOLDER_RECORD) {
        memcpy((char *) &entry->cat, (char *) &record, sizeof(hfs_folder));
    }
    else if (tsk_getu16(fs->endian, record.file.std.rec_type) == HFS_FILE_RECORD) {
        memcpy((char *) &entry->cat, (char *) &record, sizeof(hfs_file));
    }
    /* other cases already caught by hfs_cat_read_file_folder_record */

    memcpy((char *) &entry->thread, (char *) &thread, sizeof(hfs_thread));

    entry->flags = TSK_FS_META_FLAG_ALLOC | TSK_FS_META_FLAG_USED;
    entry->inum = inum;

    if (follow_hard_link) {
        // TEST to see if this is a hard link
        unsigned char is_err;
        TSK_INUM_T target_cnid =
            hfs_follow_hard_link(hfs, &(entry->cat), &is_err);
        if (is_err > 1) {
            /*
            error_returned
            ("hfs_cat_file_lookup: error occurred while following a possible hard link for "
            "inum (cnid) =  %" PRIuINUM, inum);
            */
            return 1;
        }
        if (target_cnid != inum) {
            // This is a hard link, and we have got the cnid of the target file, so look it up.
            uint8_t res =
                hfs_cat_file_lookup(hfs, target_cnid, entry, FALSE);
            if (res != 0) {
                /*
                error_returned
                ("hfs_cat_file_lookup: error occurred while looking up the Catalog entry for "
                "the target of inum (cnid) = %" PRIuINUM " target",
                inum);
                */
            }
            return 1;
        }

        // Target is NOT a hard link, so fall through to the non-hard link exit.
    }

    return 0;
}

uint8_t hfs_UTF16toUTF8(TSK_FS_INFO* fs, uint8_t* uni, int ulen, char* asc, int alen, uint32_t flags)
{
    UTF8 *ptr8;
    uint8_t *uniclean;
    UTF16 *ptr16;
    int i;
    TSKConversionResult r;

    // remove nulls from the Unicode string
    // convert / to :
    uniclean = (uint8_t *) tsk_malloc(ulen * 2);
    if (!uniclean)
        return 1;

    memcpy(uniclean, uni, ulen * 2);

    for (i = 0; i < ulen; ++i) {
        uint16_t uc = tsk_getu16(fs->endian, uniclean + i * 2);


        int changed = 0;
        if (uc == UTF16_NULL) {
            uc = UTF16_NULL_REPLACE;
            changed = 1;
        }
        else if ((flags & HFS_U16U8_FLAG_REPLACE_SLASH)
            && uc == UTF16_SLASH) {
            uc = UTF16_COLON;
            changed = 1;
        }

        else if ((flags & HFS_U16U8_FLAG_REPLACE_CONTROL)
            && uc < UTF16_LEAST_PRINTABLE) {
            uc = (uint16_t) UTF16_NULL_REPLACE;
            changed = 1;
        }

        if (changed)
            *((uint16_t *) (uniclean + i * 2)) =
                tsk_getu16(fs->endian, (uint8_t *) & uc);
    }

    // convert to UTF-8
    memset(asc, 0, alen);

    ptr8 = (UTF8 *) asc;
    ptr16 = (UTF16 *) uniclean;
    r = tsk_UTF16toUTF8(fs->endian, (const UTF16 **) &ptr16,
    (const UTF16 *) (&uniclean[ulen * 2]), &ptr8,
    (UTF8 *) & asc[alen], TSKstrictConversion);

    free(uniclean);
    if (r != TSKconversionOK) {
        tsk_error_set_errno(TSK_ERR_FS_UNICODE);
        tsk_error_set_errstr
            ("hfs_UTF16toUTF8: unicode conversion failed (%d)", (int) r);
        return 1;
    }

    return 0;
}
    static uint8_t hfs_cat_get_record_offset_cb(HFS_INFO* hfs, int8_t level_type, const void* targ_data, const hfs_btree_key_cat* cur_key, TSK_OFF_T key_off, void* ptr)
    {
        const hfs_btree_key_cat *targ_key = (hfs_btree_key_cat *) targ_data;
        //if (tsk_verbose)
        //    tsk_fprintf(stderr,
        //        "hfs_cat_get_record_offset_cb: %s node want: %" PRIu32
        //        " vs have: %" PRIu32 "\n",
        //        (level_type == HFS_BT_NODE_TYPE_IDX) ? "Index" : "Leaf",
        //        tsk_getu32(hfs->fs_info.endian, targ_key->parent_cnid),
        //        tsk_getu32(hfs->fs_info.endian, cur_key->parent_cnid));

        if (level_type == HFS_BT_NODE_TYPE_IDX) {
            int diff = hfs_cat_compare_keys(hfs, cur_key, targ_key);
            if (diff < 0)
                return HFS_BTREE_CB_IDX_LT;
            else
                return HFS_BTREE_CB_IDX_EQGT;
        }
        else {
            int diff = hfs_cat_compare_keys(hfs, cur_key, targ_key);

            // see if this record is for our file or if we passed the interesting entries
            if (diff < 0) {
                return HFS_BTREE_CB_LEAF_GO;
            }
            else if (diff == 0) {
                TSK_OFF_T *off = (TSK_OFF_T *) ptr;
                *off =
                    key_off + 2 + tsk_getu16(hfs->fs_info.endian,
                    cur_key->key_len);
            }
            return HFS_BTREE_CB_LEAF_STOP;
        }   
    }
    static TSK_OFF_T hfs_cat_get_record_offset(HFS_INFO* hfs, const hfs_btree_key_cat* needle)
    {
        TSK_OFF_T off = 0;
        if(hfs_cat_traverse(hfs, needle, hfs_cat_get_record_offset_cb, &off))
        {
            return 0;
        }
        return off;
    }
    uint8_t hfs_cat_read_thread_record(HFS_INFO* hfs, TSK_OFF_T off, hfs_thread* thread)
    {
        TSK_FS_INFO *fs = (TSK_FS_INFO *) & (hfs->fs_info);
        uint16_t uni_len;
        size_t cnt;

        memset(thread, 0, sizeof(hfs_thread));
        cnt = tsk_fs_attr_read(hfs->catalog_attr, off, (char *) thread, 10, (TSK_FS_FILE_READ_FLAG_ENUM)0);
        if (cnt != 10) {
            //tsk_error_reset();
            //tsk_error_set_errno(TSK_ERR_FS_READ);
            //tsk_error_set_errstr2("hfs_cat_read_thread_record: Error reading catalog offset %"PRIuOFF " (header)", off);
            return 1;
        }

        if ((tsk_getu16(fs->endian, thread->rec_type) != HFS_FOLDER_THREAD)
            && (tsk_getu16(fs->endian, thread->rec_type) != HFS_FILE_THREAD)) {
            //tsk_error_set_errno(TSK_ERR_FS_GENFS);
            //tsk_error_set_errstr("hfs_cat_read_thread_record: unexpected record type %" PRIu16,tsk_getu16(fs->endian, thread->rec_type));
            return 1;
        }

        uni_len = tsk_getu16(fs->endian, thread->name.length);

        if (uni_len > 255) {
            //tsk_error_set_errno(TSK_ERR_FS_INODE_COR);
            //tsk_error_set_errstr
            //("hfs_cat_read_thread_record: invalid string length (%" PRIu16
            //")", uni_len);
            return 1;
        }

        cnt =
            tsk_fs_attr_read(hfs->catalog_attr, off + 10, (char *) thread->name.unicode, uni_len * 2, (TSK_FS_FILE_READ_FLAG_ENUM)0);
        if (cnt != uni_len * 2) {
                //tsk_error_reset();
            //tsk_error_set_errno(TSK_ERR_FS_READ);
            //tsk_error_set_errstr2
            //("hfs_cat_read_thread_record: Error reading catalog offset %"
            //PRIuOFF " (name)", off + 10);
            return 1;
        }

        return 0;
    }
    uint8_t hfs_cat_read_file_folder_record(HFS_INFO* hfs, TSK_OFF_T off, hfs_file_folder* record)
    {
        TSK_FS_INFO *fs = (TSK_FS_INFO *) & (hfs->fs_info);
        size_t cnt;
        char rec_type[2];

        memset(record, 0, sizeof(hfs_file_folder));

        cnt = tsk_fs_attr_read(hfs->catalog_attr, off, rec_type, 2, (TSK_FS_FILE_READ_FLAG_ENUM)0);
        if (cnt != 2) {
            //tsk_error_reset();
            //tsk_error_set_errno(TSK_ERR_FS_READ);
            //tsk_error_set_errstr2
            //("hfs_cat_read_file_folder_record: Error reading record type from catalog offset %"
            //PRIuOFF " (header)", off);
            return 1;
        }

        if (tsk_getu16(fs->endian, rec_type) == HFS_FOLDER_RECORD) {
            cnt = 
                tsk_fs_attr_read(hfs->catalog_attr, off, (char *) record, sizeof(hfs_folder), (TSK_FS_FILE_READ_FLAG_ENUM)0);
            if (cnt != sizeof(hfs_folder)) {
                //tsk_error_reset();
                //tsk_error_set_errno(TSK_ERR_FS_READ);
                //tsk_error_set_errstr2
                //   ("hfs_cat_read_file_folder_record: Error reading catalog offset %"
                //    PRIuOFF " (folder)", off);
                return 1;
            }
        }
        else if (tsk_getu16(fs->endian, rec_type) == HFS_FILE_RECORD) {
            cnt =
                tsk_fs_attr_read(hfs->catalog_attr, off, (char *) record, sizeof(hfs_file), (TSK_FS_FILE_READ_FLAG_ENUM)0);
            if (cnt != sizeof(hfs_file)) {
                //tsk_error_reset();
                //tsk_error_set_errno(TSK_ERR_FS_READ);
            //tsk_error_set_errstr2
            //    ("hfs_cat_read_file_folder_record: Error reading catalog offset %"
            //    PRIuOFF " (file)", off);
            return 1;
            }
        }
        else {
            //tsk_error_set_errno(TSK_ERR_FS_GENFS);
            //tsk_error_set_errstr
            //    ("hfs_cat_read_file_folder_record: unexpected record type %"
            //    PRIu16, tsk_getu16(fs->endian, rec_type));
            return 1;
        }

        return 0;
    }
    static int hfs_cat_compare_keys(HFS_INFO* hfs, const hfs_btree_key_cat* key1, const hfs_btree_key_cat* key2)
    {
        TSK_FS_INFO *fs = (TSK_FS_INFO *) & (hfs->fs_info);
        uint32_t cnid1, cnid2;

        cnid1 = tsk_getu32(fs->endian, key1->parent_cnid);
        cnid2 = tsk_getu32(fs->endian, key2->parent_cnid);

        if (cnid1 < cnid2)
            return -1;
        if (cnid1 > cnid2)
            return 1;

        return hfs_unicode_compare(hfs, &key1->name, &key2->name);

    }
    uint8_t hfs_cat_traverse(HFS_INFO* hfs, const void* targ_data, TSK_HFS_BTREE_CB a_cb, void* ptr)
    {
        TSK_FS_INFO *fs = &(hfs->fs_info);
        uint32_t cur_node;          /* node id of the current node */
        char *node;

        uint16_t nodesize;
        uint8_t is_done = 0;

        //tsk_error_reset();

        nodesize = tsk_getu16(fs->endian, hfs->catalog_header.nodesize);
        if ((node = (char *) tsk_malloc(nodesize)) == NULL)
            return 1;

        /* start at root node */
        cur_node = tsk_getu32(fs->endian, hfs->catalog_header.rootNode);

        /* if the root node is zero, then the extents btree is empty */
        /* if no files have overflow extents, the Extents B-tree still
           exists on disk, but is an empty B-tree containing only
           the header node */
        if (cur_node == 0) {
            //if (tsk_verbose)
            //    tsk_fprintf(stderr, "hfs_cat_traverse: "
            //        "empty extents btree\n");
            free(node);
            return 1;
        }

        //if (tsk_verbose)
        //    tsk_fprintf(stderr, "hfs_cat_traverse: starting at "
        //        "root node %" PRIu32 "; nodesize = %"
        //        PRIu16 "\n", cur_node, nodesize);

        /* Recurse down to the needed leaf nodes and then go forward */
        is_done = 0;
        while (is_done == 0) {
            TSK_OFF_T cur_off;      /* start address of cur_node */
            uint16_t num_rec;       /* number of records in this node */
            ssize_t cnt;
            hfs_btree_node *node_desc;

            // sanity check 
            if (cur_node > tsk_getu32(fs->endian,
                    hfs->catalog_header.totalNodes)) {
         //       tsk_error_set_errno(TSK_ERR_FS_GENFS);
         //       tsk_error_set_errstr
         //           ("hfs_cat_traverse: Node %d too large for file", cur_node);
                free(node);
                return 1;
            }

            // read the current node
            cur_off = cur_node * nodesize;
            cnt = tsk_fs_attr_read(hfs->catalog_attr, cur_off, node, nodesize, (TSK_FS_FILE_READ_FLAG_ENUM)0);
            if (cnt != nodesize) {
                if (cnt >= 0) {
         //           tsk_error_reset();
         //           tsk_error_set_errno(TSK_ERR_FS_READ);
                }
         //       tsk_error_set_errstr2
         //           ("hfs_cat_traverse: Error reading node %d at offset %"
         //           PRIuOFF, cur_node, cur_off);
                free(node);
                return 1;
            }

            // process the header / descriptor
            node_desc = (hfs_btree_node *) node;
            num_rec = tsk_getu16(fs->endian, node_desc->num_rec);

         //   if (tsk_verbose)
         //       tsk_fprintf(stderr, "hfs_cat_traverse: node %" PRIu32
         //           " @ %" PRIu64 " has %" PRIu16 " records\n",
         //           cur_node, cur_off, num_rec);

            if (num_rec == 0) {
         //       tsk_error_set_errno(TSK_ERR_FS_GENFS);
         //       tsk_error_set_errstr("hfs_cat_traverse: zero records in node %"
         //           PRIu32, cur_node);
                free(node);
                return 1;
            }

            /* With an index node, find the record with the largest key that is smaller
             * to or equal to cnid */
            if (node_desc->type == HFS_BT_NODE_TYPE_IDX) {
                uint32_t next_node = 0;
                int rec;

                for (rec = 0; rec < num_rec; rec++) {
                    size_t rec_off;
                    hfs_btree_key_cat *key;
                    uint8_t retval;
    
                    // get the record offset in the node
                    rec_off =
                        tsk_getu16(fs->endian,
                        &node[nodesize - (rec + 1) * 2]);
                    if (rec_off > nodesize) {
           //             tsk_error_set_errno(TSK_ERR_FS_GENFS);
           //             tsk_error_set_errstr
           //                 ("hfs_cat_traverse: offset of record %d in index node %d too large (%d vs %"
           //                 PRIu16 ")", rec, cur_node, (int) rec_off,
           //                 nodesize);
                        free(node);
                        return 1;
                    }
                    key = (hfs_btree_key_cat *) & node[rec_off];

                    /*
                       if (tsk_verbose)
                       tsk_fprintf(stderr,
                       "hfs_cat_traverse: record %" PRIu16
                       " ; keylen %" PRIu16 " (%" PRIu32 ")\n", rec,
                       tsk_getu16(fs->endian, key->key_len),
                       tsk_getu32(fs->endian, key->parent_cnid));
                     */

                    /* save the info from this record unless it is too big */
                    retval =
                        a_cb(hfs, HFS_BT_NODE_TYPE_IDX, targ_data, key,
                        cur_off + rec_off, ptr);
                    if (retval == HFS_BTREE_CB_ERR) {
         //               tsk_error_set_errno(TSK_ERR_FS_GENFS);
         //               tsk_error_set_errstr2
         //                   ("hfs_cat_traverse: Callback returned error");
                        free(node);
                        return 1;
                    }
                    // record the closest entry
                    else if ((retval == HFS_BTREE_CB_IDX_LT)
                        || (next_node == 0)) {
                        hfs_btree_index_record *idx_rec;
                        int keylen =
                            2 + hfs_get_idxkeylen(hfs, tsk_getu16(fs->endian,
                                key->key_len), &(hfs->catalog_header));
                        if (rec_off + keylen > nodesize) {
         //                   tsk_error_set_errno(TSK_ERR_FS_GENFS);
         //                   tsk_error_set_errstr
         //                       ("hfs_cat_traverse: offset of record and keylength %d in index node %d too large (%d vs %"
         //                       PRIu16 ")", rec, cur_node,
         //                       (int) rec_off + keylen, nodesize);
                            free(node);
                            return 1;
                        }
                        idx_rec =
                            (hfs_btree_index_record *) & node[rec_off +
                            keylen];
                        next_node = tsk_getu32(fs->endian, idx_rec->childNode);
                    }
                    if (retval == HFS_BTREE_CB_IDX_EQGT) {
                        // move down to the next node
                        break;
                    }
                }
                // check if we found a relevant node
                if (next_node == 0) {
         //           tsk_error_set_errno(TSK_ERR_FS_GENFS);
         //           tsk_error_set_errstr
         //               ("hfs_cat_traverse: did not find any keys in index node %d",
         //               cur_node);
                    is_done = 1;
                    break;
                }
                cur_node = next_node;
            }

            /* With a leaf, we look for the specific record. */
            else if (node_desc->type == HFS_BT_NODE_TYPE_LEAF) {
                int rec;

            for (rec = 0; rec < num_rec; rec++) {
                size_t rec_off;
                hfs_btree_key_cat *key;
                uint8_t retval;

                // get the record offset in the node
                rec_off =
                    tsk_getu16(fs->endian,
                    &node[nodesize - (rec + 1) * 2]);
                if (rec_off > nodesize) {
      //              tsk_error_set_errno(TSK_ERR_FS_GENFS);
      //              tsk_error_set_errstr
      //                  ("hfs_cat_traverse: offset of record %d in leaf node %d too large (%d vs %"
      //                  PRIu16 ")", rec, cur_node, (int) rec_off,
      //                  nodesize);
                    free(node);
                    return 1;
                }
                key = (hfs_btree_key_cat *) & node[rec_off];

                /*
                   if (tsk_verbose)
                   tsk_fprintf(stderr,
                   "hfs_cat_traverse: record %" PRIu16
                   "; keylen %" PRIu16 " (%" PRIu32 ")\n", rec,
                   tsk_getu16(fs->endian, key->key_len),
                   tsk_getu32(fs->endian, key->parent_cnid));
                 */
                //                rec_cnid = tsk_getu32(fs->endian, key->file_id);

                retval =
                    a_cb(hfs, HFS_BT_NODE_TYPE_LEAF, targ_data, key,
                    cur_off + rec_off, ptr);
                if (retval == HFS_BTREE_CB_LEAF_STOP) {
                    is_done = 1;
                    break;
                }
                else if (retval == HFS_BTREE_CB_ERR) {
      //              tsk_error_set_errno(TSK_ERR_FS_GENFS);
      //              tsk_error_set_errstr2
      //                  ("hfs_cat_traverse: Callback returned error");
                    free(node);
                    return 1;
                }
            }

            // move right to the next node if we got this far
            if (is_done == 0) {
                cur_node = tsk_getu32(fs->endian, node_desc->flink);
                if (cur_node == 0) {
                    is_done = 1;
                }
      //          if (tsk_verbose)
      //              tsk_fprintf(stderr,
      //                  "hfs_cat_traverse: moving forward to next leaf");
              }
            }
            else {
          //      tsk_error_set_errno(TSK_ERR_FS_GENFS);
          //      tsk_error_set_errstr("hfs_cat_traverse: btree node %" PRIu32
          //          " (%" PRIu64 ") is neither index nor leaf (%" PRIu8 ")",
          //          cur_node, cur_off, node_desc->type);
                free(node);
                return 1;
            }
        }
        free(node);
        return 0;
    }

uint16_t hfs_get_idxkeylen(HFS_INFO * hfs, uint16_t keylen, const hfs_btree_header_record * header)
{
    TSK_FS_INFO *fs = (TSK_FS_INFO *) & (hfs->fs_info);

    // if the flag is set, use the length given in the record
    if (tsk_getu32(fs->endian, header->attr) & HFS_BT_HEAD_ATTR_VARIDXKEYS)
        return keylen;
    else
        return tsk_getu16(fs->endian, header->maxKeyLen);
}

uint16_t gLowerCaseTable[] = {

    // High-byte indices ( == 0 iff no case mapping and no ignorables )


    /* 0 */ 0x0100, 0x0200, 0x0000, 0x0300, 0x0400, 0x0500, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 1 */ 0x0600, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 2 */ 0x0700, 0x0800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 3 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 4 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 5 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 6 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 7 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 8 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 9 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* A */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* B */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* C */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* D */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* E */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* F */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0900, 0x0A00,

    // Table 1 (for high byte 0x00)

    /* 0 */ 0xFFFF, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    /* 1 */ 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    /* 2 */ 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    /* 3 */ 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    /* 4 */ 0x0040, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    /* 5 */ 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    /* 6 */ 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    /* 7 */ 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    /* 8 */ 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    /* 9 */ 0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
    /* A */ 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
    /* B */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
    0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
    /* C */ 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00E6, 0x00C7,
    0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
    /* D */ 0x00F0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
    0x00F8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00FE, 0x00DF,
    /* E */ 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
    0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
    /* F */ 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
    0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,

    // Table 2 (for high byte 0x01)

    /* 0 */ 0x0100, 0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107,
    0x0108, 0x0109, 0x010A, 0x010B, 0x010C, 0x010D, 0x010E, 0x010F,
    /* 1 */ 0x0111, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117,
    0x0118, 0x0119, 0x011A, 0x011B, 0x011C, 0x011D, 0x011E, 0x011F,
    /* 2 */ 0x0120, 0x0121, 0x0122, 0x0123, 0x0124, 0x0125, 0x0127, 0x0127,
    0x0128, 0x0129, 0x012A, 0x012B, 0x012C, 0x012D, 0x012E, 0x012F,
    /* 3 */ 0x0130, 0x0131, 0x0133, 0x0133, 0x0134, 0x0135, 0x0136, 0x0137,
    0x0138, 0x0139, 0x013A, 0x013B, 0x013C, 0x013D, 0x013E, 0x0140,
    /* 4 */ 0x0140, 0x0142, 0x0142, 0x0143, 0x0144, 0x0145, 0x0146, 0x0147,
    0x0148, 0x0149, 0x014B, 0x014B, 0x014C, 0x014D, 0x014E, 0x014F,
    /* 5 */ 0x0150, 0x0151, 0x0153, 0x0153, 0x0154, 0x0155, 0x0156, 0x0157,
    0x0158, 0x0159, 0x015A, 0x015B, 0x015C, 0x015D, 0x015E, 0x015F,
    /* 6 */ 0x0160, 0x0161, 0x0162, 0x0163, 0x0164, 0x0165, 0x0167, 0x0167,
    0x0168, 0x0169, 0x016A, 0x016B, 0x016C, 0x016D, 0x016E, 0x016F,
    /* 7 */ 0x0170, 0x0171, 0x0172, 0x0173, 0x0174, 0x0175, 0x0176, 0x0177,
    0x0178, 0x0179, 0x017A, 0x017B, 0x017C, 0x017D, 0x017E, 0x017F,
    /* 8 */ 0x0180, 0x0253, 0x0183, 0x0183, 0x0185, 0x0185, 0x0254, 0x0188,
    0x0188, 0x0256, 0x0257, 0x018C, 0x018C, 0x018D, 0x01DD, 0x0259,
    /* 9 */ 0x025B, 0x0192, 0x0192, 0x0260, 0x0263, 0x0195, 0x0269, 0x0268,
    0x0199, 0x0199, 0x019A, 0x019B, 0x026F, 0x0272, 0x019E, 0x0275,
    /* A */ 0x01A0, 0x01A1, 0x01A3, 0x01A3, 0x01A5, 0x01A5, 0x01A6, 0x01A8,
    0x01A8, 0x0283, 0x01AA, 0x01AB, 0x01AD, 0x01AD, 0x0288, 0x01AF,
    /* B */ 0x01B0, 0x028A, 0x028B, 0x01B4, 0x01B4, 0x01B6, 0x01B6, 0x0292,
    0x01B9, 0x01B9, 0x01BA, 0x01BB, 0x01BD, 0x01BD, 0x01BE, 0x01BF,
    /* C */ 0x01C0, 0x01C1, 0x01C2, 0x01C3, 0x01C6, 0x01C6, 0x01C6, 0x01C9,
    0x01C9, 0x01C9, 0x01CC, 0x01CC, 0x01CC, 0x01CD, 0x01CE, 0x01CF,
    /* D */ 0x01D0, 0x01D1, 0x01D2, 0x01D3, 0x01D4, 0x01D5, 0x01D6, 0x01D7,
    0x01D8, 0x01D9, 0x01DA, 0x01DB, 0x01DC, 0x01DD, 0x01DE, 0x01DF,
    /* E */ 0x01E0, 0x01E1, 0x01E2, 0x01E3, 0x01E5, 0x01E5, 0x01E6, 0x01E7,
    0x01E8, 0x01E9, 0x01EA, 0x01EB, 0x01EC, 0x01ED, 0x01EE, 0x01EF,
    /* F */ 0x01F0, 0x01F3, 0x01F3, 0x01F3, 0x01F4, 0x01F5, 0x01F6, 0x01F7,
    0x01F8, 0x01F9, 0x01FA, 0x01FB, 0x01FC, 0x01FD, 0x01FE, 0x01FF,

    // Table 3 (for high byte 0x03)

    /* 0 */ 0x0300, 0x0301, 0x0302, 0x0303, 0x0304, 0x0305, 0x0306, 0x0307,
    0x0308, 0x0309, 0x030A, 0x030B, 0x030C, 0x030D, 0x030E, 0x030F,
    /* 1 */ 0x0310, 0x0311, 0x0312, 0x0313, 0x0314, 0x0315, 0x0316, 0x0317,
    0x0318, 0x0319, 0x031A, 0x031B, 0x031C, 0x031D, 0x031E, 0x031F,
    /* 2 */ 0x0320, 0x0321, 0x0322, 0x0323, 0x0324, 0x0325, 0x0326, 0x0327,
    0x0328, 0x0329, 0x032A, 0x032B, 0x032C, 0x032D, 0x032E, 0x032F,
    /* 3 */ 0x0330, 0x0331, 0x0332, 0x0333, 0x0334, 0x0335, 0x0336, 0x0337,
    0x0338, 0x0339, 0x033A, 0x033B, 0x033C, 0x033D, 0x033E, 0x033F,
    /* 4 */ 0x0340, 0x0341, 0x0342, 0x0343, 0x0344, 0x0345, 0x0346, 0x0347,
    0x0348, 0x0349, 0x034A, 0x034B, 0x034C, 0x034D, 0x034E, 0x034F,
    /* 5 */ 0x0350, 0x0351, 0x0352, 0x0353, 0x0354, 0x0355, 0x0356, 0x0357,
    0x0358, 0x0359, 0x035A, 0x035B, 0x035C, 0x035D, 0x035E, 0x035F,
    /* 6 */ 0x0360, 0x0361, 0x0362, 0x0363, 0x0364, 0x0365, 0x0366, 0x0367,
    0x0368, 0x0369, 0x036A, 0x036B, 0x036C, 0x036D, 0x036E, 0x036F,
    /* 7 */ 0x0370, 0x0371, 0x0372, 0x0373, 0x0374, 0x0375, 0x0376, 0x0377,
    0x0378, 0x0379, 0x037A, 0x037B, 0x037C, 0x037D, 0x037E, 0x037F,
    /* 8 */ 0x0380, 0x0381, 0x0382, 0x0383, 0x0384, 0x0385, 0x0386, 0x0387,
    0x0388, 0x0389, 0x038A, 0x038B, 0x038C, 0x038D, 0x038E, 0x038F,
    /* 9 */ 0x0390, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
    0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
    /* A */ 0x03C0, 0x03C1, 0x03A2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
    0x03C8, 0x03C9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
    /* B */ 0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
    0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
    /* C */ 0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
    0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, 0x03CF,
    /* D */ 0x03D0, 0x03D1, 0x03D2, 0x03D3, 0x03D4, 0x03D5, 0x03D6, 0x03D7,
    0x03D8, 0x03D9, 0x03DA, 0x03DB, 0x03DC, 0x03DD, 0x03DE, 0x03DF,
    /* E */ 0x03E0, 0x03E1, 0x03E3, 0x03E3, 0x03E5, 0x03E5, 0x03E7, 0x03E7,
    0x03E9, 0x03E9, 0x03EB, 0x03EB, 0x03ED, 0x03ED, 0x03EF, 0x03EF,
    /* F */ 0x03F0, 0x03F1, 0x03F2, 0x03F3, 0x03F4, 0x03F5, 0x03F6, 0x03F7,
    0x03F8, 0x03F9, 0x03FA, 0x03FB, 0x03FC, 0x03FD, 0x03FE, 0x03FF,

    // Table 4 (for high byte 0x04)

    /* 0 */ 0x0400, 0x0401, 0x0452, 0x0403, 0x0454, 0x0455, 0x0456, 0x0407,
    0x0458, 0x0459, 0x045A, 0x045B, 0x040C, 0x040D, 0x040E, 0x045F,
    /* 1 */ 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0419, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    /* 2 */ 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
    /* 3 */ 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    /* 4 */ 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
    /* 5 */ 0x0450, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
    0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x045D, 0x045E, 0x045F,
    /* 6 */ 0x0461, 0x0461, 0x0463, 0x0463, 0x0465, 0x0465, 0x0467, 0x0467,
    0x0469, 0x0469, 0x046B, 0x046B, 0x046D, 0x046D, 0x046F, 0x046F,
    /* 7 */ 0x0471, 0x0471, 0x0473, 0x0473, 0x0475, 0x0475, 0x0476, 0x0477,
    0x0479, 0x0479, 0x047B, 0x047B, 0x047D, 0x047D, 0x047F, 0x047F,
    /* 8 */ 0x0481, 0x0481, 0x0482, 0x0483, 0x0484, 0x0485, 0x0486, 0x0487,
    0x0488, 0x0489, 0x048A, 0x048B, 0x048C, 0x048D, 0x048E, 0x048F,
    /* 9 */ 0x0491, 0x0491, 0x0493, 0x0493, 0x0495, 0x0495, 0x0497, 0x0497,
    0x0499, 0x0499, 0x049B, 0x049B, 0x049D, 0x049D, 0x049F, 0x049F,
    /* A */ 0x04A1, 0x04A1, 0x04A3, 0x04A3, 0x04A5, 0x04A5, 0x04A7, 0x04A7,
    0x04A9, 0x04A9, 0x04AB, 0x04AB, 0x04AD, 0x04AD, 0x04AF, 0x04AF,
    /* B */ 0x04B1, 0x04B1, 0x04B3, 0x04B3, 0x04B5, 0x04B5, 0x04B7, 0x04B7,
    0x04B9, 0x04B9, 0x04BB, 0x04BB, 0x04BD, 0x04BD, 0x04BF, 0x04BF,
    /* C */ 0x04C0, 0x04C1, 0x04C2, 0x04C4, 0x04C4, 0x04C5, 0x04C6, 0x04C8,
    0x04C8, 0x04C9, 0x04CA, 0x04CC, 0x04CC, 0x04CD, 0x04CE, 0x04CF,
    /* D */ 0x04D0, 0x04D1, 0x04D2, 0x04D3, 0x04D4, 0x04D5, 0x04D6, 0x04D7,
    0x04D8, 0x04D9, 0x04DA, 0x04DB, 0x04DC, 0x04DD, 0x04DE, 0x04DF,
    /* E */ 0x04E0, 0x04E1, 0x04E2, 0x04E3, 0x04E4, 0x04E5, 0x04E6, 0x04E7,
    0x04E8, 0x04E9, 0x04EA, 0x04EB, 0x04EC, 0x04ED, 0x04EE, 0x04EF,
    /* F */ 0x04F0, 0x04F1, 0x04F2, 0x04F3, 0x04F4, 0x04F5, 0x04F6, 0x04F7,
    0x04F8, 0x04F9, 0x04FA, 0x04FB, 0x04FC, 0x04FD, 0x04FE, 0x04FF,

    // Table 5 (for high byte 0x05)

    /* 0 */ 0x0500, 0x0501, 0x0502, 0x0503, 0x0504, 0x0505, 0x0506, 0x0507,
    0x0508, 0x0509, 0x050A, 0x050B, 0x050C, 0x050D, 0x050E, 0x050F,
    /* 1 */ 0x0510, 0x0511, 0x0512, 0x0513, 0x0514, 0x0515, 0x0516, 0x0517,
    0x0518, 0x0519, 0x051A, 0x051B, 0x051C, 0x051D, 0x051E, 0x051F,
    /* 2 */ 0x0520, 0x0521, 0x0522, 0x0523, 0x0524, 0x0525, 0x0526, 0x0527,
    0x0528, 0x0529, 0x052A, 0x052B, 0x052C, 0x052D, 0x052E, 0x052F,
    /* 3 */ 0x0530, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567,
    0x0568, 0x0569, 0x056A, 0x056B, 0x056C, 0x056D, 0x056E, 0x056F,
    /* 4 */ 0x0570, 0x0571, 0x0572, 0x0573, 0x0574, 0x0575, 0x0576, 0x0577,
    0x0578, 0x0579, 0x057A, 0x057B, 0x057C, 0x057D, 0x057E, 0x057F,
    /* 5 */ 0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586, 0x0557,
    0x0558, 0x0559, 0x055A, 0x055B, 0x055C, 0x055D, 0x055E, 0x055F,
    /* 6 */ 0x0560, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567,
    0x0568, 0x0569, 0x056A, 0x056B, 0x056C, 0x056D, 0x056E, 0x056F,
    /* 7 */ 0x0570, 0x0571, 0x0572, 0x0573, 0x0574, 0x0575, 0x0576, 0x0577,
    0x0578, 0x0579, 0x057A, 0x057B, 0x057C, 0x057D, 0x057E, 0x057F,
    /* 8 */ 0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586, 0x0587,
    0x0588, 0x0589, 0x058A, 0x058B, 0x058C, 0x058D, 0x058E, 0x058F,
    /* 9 */ 0x0590, 0x0591, 0x0592, 0x0593, 0x0594, 0x0595, 0x0596, 0x0597,
    0x0598, 0x0599, 0x059A, 0x059B, 0x059C, 0x059D, 0x059E, 0x059F,
    /* A */ 0x05A0, 0x05A1, 0x05A2, 0x05A3, 0x05A4, 0x05A5, 0x05A6, 0x05A7,
    0x05A8, 0x05A9, 0x05AA, 0x05AB, 0x05AC, 0x05AD, 0x05AE, 0x05AF,
    /* B */ 0x05B0, 0x05B1, 0x05B2, 0x05B3, 0x05B4, 0x05B5, 0x05B6, 0x05B7,
    0x05B8, 0x05B9, 0x05BA, 0x05BB, 0x05BC, 0x05BD, 0x05BE, 0x05BF,
    /* C */ 0x05C0, 0x05C1, 0x05C2, 0x05C3, 0x05C4, 0x05C5, 0x05C6, 0x05C7,
    0x05C8, 0x05C9, 0x05CA, 0x05CB, 0x05CC, 0x05CD, 0x05CE, 0x05CF,
    /* D */ 0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
    0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
    /* E */ 0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
    0x05E8, 0x05E9, 0x05EA, 0x05EB, 0x05EC, 0x05ED, 0x05EE, 0x05EF,
    /* F */ 0x05F0, 0x05F1, 0x05F2, 0x05F3, 0x05F4, 0x05F5, 0x05F6, 0x05F7,
    0x05F8, 0x05F9, 0x05FA, 0x05FB, 0x05FC, 0x05FD, 0x05FE, 0x05FF,

    // Table 6 (for high byte 0x10)

    /* 0 */ 0x1000, 0x1001, 0x1002, 0x1003, 0x1004, 0x1005, 0x1006, 0x1007,
    0x1008, 0x1009, 0x100A, 0x100B, 0x100C, 0x100D, 0x100E, 0x100F,
    /* 1 */ 0x1010, 0x1011, 0x1012, 0x1013, 0x1014, 0x1015, 0x1016, 0x1017,
    0x1018, 0x1019, 0x101A, 0x101B, 0x101C, 0x101D, 0x101E, 0x101F,
    /* 2 */ 0x1020, 0x1021, 0x1022, 0x1023, 0x1024, 0x1025, 0x1026, 0x1027,
    0x1028, 0x1029, 0x102A, 0x102B, 0x102C, 0x102D, 0x102E, 0x102F,
    /* 3 */ 0x1030, 0x1031, 0x1032, 0x1033, 0x1034, 0x1035, 0x1036, 0x1037,
    0x1038, 0x1039, 0x103A, 0x103B, 0x103C, 0x103D, 0x103E, 0x103F,
    /* 4 */ 0x1040, 0x1041, 0x1042, 0x1043, 0x1044, 0x1045, 0x1046, 0x1047,
    0x1048, 0x1049, 0x104A, 0x104B, 0x104C, 0x104D, 0x104E, 0x104F,
    /* 5 */ 0x1050, 0x1051, 0x1052, 0x1053, 0x1054, 0x1055, 0x1056, 0x1057,
    0x1058, 0x1059, 0x105A, 0x105B, 0x105C, 0x105D, 0x105E, 0x105F,
    /* 6 */ 0x1060, 0x1061, 0x1062, 0x1063, 0x1064, 0x1065, 0x1066, 0x1067,
    0x1068, 0x1069, 0x106A, 0x106B, 0x106C, 0x106D, 0x106E, 0x106F,
    /* 7 */ 0x1070, 0x1071, 0x1072, 0x1073, 0x1074, 0x1075, 0x1076, 0x1077,
    0x1078, 0x1079, 0x107A, 0x107B, 0x107C, 0x107D, 0x107E, 0x107F,
    /* 8 */ 0x1080, 0x1081, 0x1082, 0x1083, 0x1084, 0x1085, 0x1086, 0x1087,
    0x1088, 0x1089, 0x108A, 0x108B, 0x108C, 0x108D, 0x108E, 0x108F,
    /* 9 */ 0x1090, 0x1091, 0x1092, 0x1093, 0x1094, 0x1095, 0x1096, 0x1097,
    0x1098, 0x1099, 0x109A, 0x109B, 0x109C, 0x109D, 0x109E, 0x109F,
    /* A */ 0x10D0, 0x10D1, 0x10D2, 0x10D3, 0x10D4, 0x10D5, 0x10D6, 0x10D7,
    0x10D8, 0x10D9, 0x10DA, 0x10DB, 0x10DC, 0x10DD, 0x10DE, 0x10DF,
    /* B */ 0x10E0, 0x10E1, 0x10E2, 0x10E3, 0x10E4, 0x10E5, 0x10E6, 0x10E7,
    0x10E8, 0x10E9, 0x10EA, 0x10EB, 0x10EC, 0x10ED, 0x10EE, 0x10EF,
    /* C */ 0x10F0, 0x10F1, 0x10F2, 0x10F3, 0x10F4, 0x10F5, 0x10C6, 0x10C7,
    0x10C8, 0x10C9, 0x10CA, 0x10CB, 0x10CC, 0x10CD, 0x10CE, 0x10CF,
    /* D */ 0x10D0, 0x10D1, 0x10D2, 0x10D3, 0x10D4, 0x10D5, 0x10D6, 0x10D7,
    0x10D8, 0x10D9, 0x10DA, 0x10DB, 0x10DC, 0x10DD, 0x10DE, 0x10DF,
    /* E */ 0x10E0, 0x10E1, 0x10E2, 0x10E3, 0x10E4, 0x10E5, 0x10E6, 0x10E7,
    0x10E8, 0x10E9, 0x10EA, 0x10EB, 0x10EC, 0x10ED, 0x10EE, 0x10EF,
    /* F */ 0x10F0, 0x10F1, 0x10F2, 0x10F3, 0x10F4, 0x10F5, 0x10F6, 0x10F7,
    0x10F8, 0x10F9, 0x10FA, 0x10FB, 0x10FC, 0x10FD, 0x10FE, 0x10FF,

    // Table 7 (for high byte 0x20)

    /* 0 */ 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007,
    0x2008, 0x2009, 0x200A, 0x200B, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 1 */ 0x2010, 0x2011, 0x2012, 0x2013, 0x2014, 0x2015, 0x2016, 0x2017,
    0x2018, 0x2019, 0x201A, 0x201B, 0x201C, 0x201D, 0x201E, 0x201F,
    /* 2 */ 0x2020, 0x2021, 0x2022, 0x2023, 0x2024, 0x2025, 0x2026, 0x2027,
    0x2028, 0x2029, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x202F,
    /* 3 */ 0x2030, 0x2031, 0x2032, 0x2033, 0x2034, 0x2035, 0x2036, 0x2037,
    0x2038, 0x2039, 0x203A, 0x203B, 0x203C, 0x203D, 0x203E, 0x203F,
    /* 4 */ 0x2040, 0x2041, 0x2042, 0x2043, 0x2044, 0x2045, 0x2046, 0x2047,
    0x2048, 0x2049, 0x204A, 0x204B, 0x204C, 0x204D, 0x204E, 0x204F,
    /* 5 */ 0x2050, 0x2051, 0x2052, 0x2053, 0x2054, 0x2055, 0x2056, 0x2057,
    0x2058, 0x2059, 0x205A, 0x205B, 0x205C, 0x205D, 0x205E, 0x205F,
    /* 6 */ 0x2060, 0x2061, 0x2062, 0x2063, 0x2064, 0x2065, 0x2066, 0x2067,
    0x2068, 0x2069, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    /* 7 */ 0x2070, 0x2071, 0x2072, 0x2073, 0x2074, 0x2075, 0x2076, 0x2077,
    0x2078, 0x2079, 0x207A, 0x207B, 0x207C, 0x207D, 0x207E, 0x207F,
    /* 8 */ 0x2080, 0x2081, 0x2082, 0x2083, 0x2084, 0x2085, 0x2086, 0x2087,
    0x2088, 0x2089, 0x208A, 0x208B, 0x208C, 0x208D, 0x208E, 0x208F,
    /* 9 */ 0x2090, 0x2091, 0x2092, 0x2093, 0x2094, 0x2095, 0x2096, 0x2097,
    0x2098, 0x2099, 0x209A, 0x209B, 0x209C, 0x209D, 0x209E, 0x209F,
    /* A */ 0x20A0, 0x20A1, 0x20A2, 0x20A3, 0x20A4, 0x20A5, 0x20A6, 0x20A7,
    0x20A8, 0x20A9, 0x20AA, 0x20AB, 0x20AC, 0x20AD, 0x20AE, 0x20AF,
    /* B */ 0x20B0, 0x20B1, 0x20B2, 0x20B3, 0x20B4, 0x20B5, 0x20B6, 0x20B7,
    0x20B8, 0x20B9, 0x20BA, 0x20BB, 0x20BC, 0x20BD, 0x20BE, 0x20BF,
    /* C */ 0x20C0, 0x20C1, 0x20C2, 0x20C3, 0x20C4, 0x20C5, 0x20C6, 0x20C7,
    0x20C8, 0x20C9, 0x20CA, 0x20CB, 0x20CC, 0x20CD, 0x20CE, 0x20CF,
    /* D */ 0x20D0, 0x20D1, 0x20D2, 0x20D3, 0x20D4, 0x20D5, 0x20D6, 0x20D7,
    0x20D8, 0x20D9, 0x20DA, 0x20DB, 0x20DC, 0x20DD, 0x20DE, 0x20DF,
    /* E */ 0x20E0, 0x20E1, 0x20E2, 0x20E3, 0x20E4, 0x20E5, 0x20E6, 0x20E7,
    0x20E8, 0x20E9, 0x20EA, 0x20EB, 0x20EC, 0x20ED, 0x20EE, 0x20EF,
    /* F */ 0x20F0, 0x20F1, 0x20F2, 0x20F3, 0x20F4, 0x20F5, 0x20F6, 0x20F7,
    0x20F8, 0x20F9, 0x20FA, 0x20FB, 0x20FC, 0x20FD, 0x20FE, 0x20FF,

    // Table 8 (for high byte 0x21)

    /* 0 */ 0x2100, 0x2101, 0x2102, 0x2103, 0x2104, 0x2105, 0x2106, 0x2107,
    0x2108, 0x2109, 0x210A, 0x210B, 0x210C, 0x210D, 0x210E, 0x210F,
    /* 1 */ 0x2110, 0x2111, 0x2112, 0x2113, 0x2114, 0x2115, 0x2116, 0x2117,
    0x2118, 0x2119, 0x211A, 0x211B, 0x211C, 0x211D, 0x211E, 0x211F,
    /* 2 */ 0x2120, 0x2121, 0x2122, 0x2123, 0x2124, 0x2125, 0x2126, 0x2127,
    0x2128, 0x2129, 0x212A, 0x212B, 0x212C, 0x212D, 0x212E, 0x212F,
    /* 3 */ 0x2130, 0x2131, 0x2132, 0x2133, 0x2134, 0x2135, 0x2136, 0x2137,
    0x2138, 0x2139, 0x213A, 0x213B, 0x213C, 0x213D, 0x213E, 0x213F,
    /* 4 */ 0x2140, 0x2141, 0x2142, 0x2143, 0x2144, 0x2145, 0x2146, 0x2147,
    0x2148, 0x2149, 0x214A, 0x214B, 0x214C, 0x214D, 0x214E, 0x214F,
    /* 5 */ 0x2150, 0x2151, 0x2152, 0x2153, 0x2154, 0x2155, 0x2156, 0x2157,
    0x2158, 0x2159, 0x215A, 0x215B, 0x215C, 0x215D, 0x215E, 0x215F,
    /* 6 */ 0x2170, 0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177,
    0x2178, 0x2179, 0x217A, 0x217B, 0x217C, 0x217D, 0x217E, 0x217F,
    /* 7 */ 0x2170, 0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177,
    0x2178, 0x2179, 0x217A, 0x217B, 0x217C, 0x217D, 0x217E, 0x217F,
    /* 8 */ 0x2180, 0x2181, 0x2182, 0x2183, 0x2184, 0x2185, 0x2186, 0x2187,
    0x2188, 0x2189, 0x218A, 0x218B, 0x218C, 0x218D, 0x218E, 0x218F,
    /* 9 */ 0x2190, 0x2191, 0x2192, 0x2193, 0x2194, 0x2195, 0x2196, 0x2197,
    0x2198, 0x2199, 0x219A, 0x219B, 0x219C, 0x219D, 0x219E, 0x219F,
    /* A */ 0x21A0, 0x21A1, 0x21A2, 0x21A3, 0x21A4, 0x21A5, 0x21A6, 0x21A7,
    0x21A8, 0x21A9, 0x21AA, 0x21AB, 0x21AC, 0x21AD, 0x21AE, 0x21AF,
    /* B */ 0x21B0, 0x21B1, 0x21B2, 0x21B3, 0x21B4, 0x21B5, 0x21B6, 0x21B7,
    0x21B8, 0x21B9, 0x21BA, 0x21BB, 0x21BC, 0x21BD, 0x21BE, 0x21BF,
    /* C */ 0x21C0, 0x21C1, 0x21C2, 0x21C3, 0x21C4, 0x21C5, 0x21C6, 0x21C7,
    0x21C8, 0x21C9, 0x21CA, 0x21CB, 0x21CC, 0x21CD, 0x21CE, 0x21CF,
    /* D */ 0x21D0, 0x21D1, 0x21D2, 0x21D3, 0x21D4, 0x21D5, 0x21D6, 0x21D7,
    0x21D8, 0x21D9, 0x21DA, 0x21DB, 0x21DC, 0x21DD, 0x21DE, 0x21DF,
    /* E */ 0x21E0, 0x21E1, 0x21E2, 0x21E3, 0x21E4, 0x21E5, 0x21E6, 0x21E7,
    0x21E8, 0x21E9, 0x21EA, 0x21EB, 0x21EC, 0x21ED, 0x21EE, 0x21EF,
    /* F */ 0x21F0, 0x21F1, 0x21F2, 0x21F3, 0x21F4, 0x21F5, 0x21F6, 0x21F7,
    0x21F8, 0x21F9, 0x21FA, 0x21FB, 0x21FC, 0x21FD, 0x21FE, 0x21FF,

    // Table 9 (for high byte 0xFE)

    /* 0 */ 0xFE00, 0xFE01, 0xFE02, 0xFE03, 0xFE04, 0xFE05, 0xFE06, 0xFE07,
    0xFE08, 0xFE09, 0xFE0A, 0xFE0B, 0xFE0C, 0xFE0D, 0xFE0E, 0xFE0F,
    /* 1 */ 0xFE10, 0xFE11, 0xFE12, 0xFE13, 0xFE14, 0xFE15, 0xFE16, 0xFE17,
    0xFE18, 0xFE19, 0xFE1A, 0xFE1B, 0xFE1C, 0xFE1D, 0xFE1E, 0xFE1F,
    /* 2 */ 0xFE20, 0xFE21, 0xFE22, 0xFE23, 0xFE24, 0xFE25, 0xFE26, 0xFE27,
    0xFE28, 0xFE29, 0xFE2A, 0xFE2B, 0xFE2C, 0xFE2D, 0xFE2E, 0xFE2F,
    /* 3 */ 0xFE30, 0xFE31, 0xFE32, 0xFE33, 0xFE34, 0xFE35, 0xFE36, 0xFE37,
    0xFE38, 0xFE39, 0xFE3A, 0xFE3B, 0xFE3C, 0xFE3D, 0xFE3E, 0xFE3F,
    /* 4 */ 0xFE40, 0xFE41, 0xFE42, 0xFE43, 0xFE44, 0xFE45, 0xFE46, 0xFE47,
    0xFE48, 0xFE49, 0xFE4A, 0xFE4B, 0xFE4C, 0xFE4D, 0xFE4E, 0xFE4F,
    /* 5 */ 0xFE50, 0xFE51, 0xFE52, 0xFE53, 0xFE54, 0xFE55, 0xFE56, 0xFE57,
    0xFE58, 0xFE59, 0xFE5A, 0xFE5B, 0xFE5C, 0xFE5D, 0xFE5E, 0xFE5F,
    /* 6 */ 0xFE60, 0xFE61, 0xFE62, 0xFE63, 0xFE64, 0xFE65, 0xFE66, 0xFE67,
    0xFE68, 0xFE69, 0xFE6A, 0xFE6B, 0xFE6C, 0xFE6D, 0xFE6E, 0xFE6F,
    /* 7 */ 0xFE70, 0xFE71, 0xFE72, 0xFE73, 0xFE74, 0xFE75, 0xFE76, 0xFE77,
    0xFE78, 0xFE79, 0xFE7A, 0xFE7B, 0xFE7C, 0xFE7D, 0xFE7E, 0xFE7F,
    /* 8 */ 0xFE80, 0xFE81, 0xFE82, 0xFE83, 0xFE84, 0xFE85, 0xFE86, 0xFE87,
    0xFE88, 0xFE89, 0xFE8A, 0xFE8B, 0xFE8C, 0xFE8D, 0xFE8E, 0xFE8F,
    /* 9 */ 0xFE90, 0xFE91, 0xFE92, 0xFE93, 0xFE94, 0xFE95, 0xFE96, 0xFE97,
    0xFE98, 0xFE99, 0xFE9A, 0xFE9B, 0xFE9C, 0xFE9D, 0xFE9E, 0xFE9F,
    /* A */ 0xFEA0, 0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4, 0xFEA5, 0xFEA6, 0xFEA7,
    0xFEA8, 0xFEA9, 0xFEAA, 0xFEAB, 0xFEAC, 0xFEAD, 0xFEAE, 0xFEAF,
    /* B */ 0xFEB0, 0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4, 0xFEB5, 0xFEB6, 0xFEB7,
    0xFEB8, 0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC, 0xFEBD, 0xFEBE, 0xFEBF,
    /* C */ 0xFEC0, 0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4, 0xFEC5, 0xFEC6, 0xFEC7,
    0xFEC8, 0xFEC9, 0xFECA, 0xFECB, 0xFECC, 0xFECD, 0xFECE, 0xFECF,
    /* D */ 0xFED0, 0xFED1, 0xFED2, 0xFED3, 0xFED4, 0xFED5, 0xFED6, 0xFED7,
    0xFED8, 0xFED9, 0xFEDA, 0xFEDB, 0xFEDC, 0xFEDD, 0xFEDE, 0xFEDF,
    /* E */ 0xFEE0, 0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4, 0xFEE5, 0xFEE6, 0xFEE7,
    0xFEE8, 0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC, 0xFEED, 0xFEEE, 0xFEEF,
    /* F */ 0xFEF0, 0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4, 0xFEF5, 0xFEF6, 0xFEF7,
    0xFEF8, 0xFEF9, 0xFEFA, 0xFEFB, 0xFEFC, 0xFEFD, 0xFEFE, 0x0000,

    // Table 10 (for high byte 0xFF)

    /* 0 */ 0xFF00, 0xFF01, 0xFF02, 0xFF03, 0xFF04, 0xFF05, 0xFF06, 0xFF07,
    0xFF08, 0xFF09, 0xFF0A, 0xFF0B, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF0F,
    /* 1 */ 0xFF10, 0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16, 0xFF17,
    0xFF18, 0xFF19, 0xFF1A, 0xFF1B, 0xFF1C, 0xFF1D, 0xFF1E, 0xFF1F,
    /* 2 */ 0xFF20, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47,
    0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F,
    /* 3 */ 0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57,
    0xFF58, 0xFF59, 0xFF5A, 0xFF3B, 0xFF3C, 0xFF3D, 0xFF3E, 0xFF3F,
    /* 4 */ 0xFF40, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47,
    0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F,
    /* 5 */ 0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57,
    0xFF58, 0xFF59, 0xFF5A, 0xFF5B, 0xFF5C, 0xFF5D, 0xFF5E, 0xFF5F,
    /* 6 */ 0xFF60, 0xFF61, 0xFF62, 0xFF63, 0xFF64, 0xFF65, 0xFF66, 0xFF67,
    0xFF68, 0xFF69, 0xFF6A, 0xFF6B, 0xFF6C, 0xFF6D, 0xFF6E, 0xFF6F,
    /* 7 */ 0xFF70, 0xFF71, 0xFF72, 0xFF73, 0xFF74, 0xFF75, 0xFF76, 0xFF77,
    0xFF78, 0xFF79, 0xFF7A, 0xFF7B, 0xFF7C, 0xFF7D, 0xFF7E, 0xFF7F,
    /* 8 */ 0xFF80, 0xFF81, 0xFF82, 0xFF83, 0xFF84, 0xFF85, 0xFF86, 0xFF87,
    0xFF88, 0xFF89, 0xFF8A, 0xFF8B, 0xFF8C, 0xFF8D, 0xFF8E, 0xFF8F,
    /* 9 */ 0xFF90, 0xFF91, 0xFF92, 0xFF93, 0xFF94, 0xFF95, 0xFF96, 0xFF97,
    0xFF98, 0xFF99, 0xFF9A, 0xFF9B, 0xFF9C, 0xFF9D, 0xFF9E, 0xFF9F,
    /* A */ 0xFFA0, 0xFFA1, 0xFFA2, 0xFFA3, 0xFFA4, 0xFFA5, 0xFFA6, 0xFFA7,
    0xFFA8, 0xFFA9, 0xFFAA, 0xFFAB, 0xFFAC, 0xFFAD, 0xFFAE, 0xFFAF,
    /* B */ 0xFFB0, 0xFFB1, 0xFFB2, 0xFFB3, 0xFFB4, 0xFFB5, 0xFFB6, 0xFFB7,
    0xFFB8, 0xFFB9, 0xFFBA, 0xFFBB, 0xFFBC, 0xFFBD, 0xFFBE, 0xFFBF,
    /* C */ 0xFFC0, 0xFFC1, 0xFFC2, 0xFFC3, 0xFFC4, 0xFFC5, 0xFFC6, 0xFFC7,
    0xFFC8, 0xFFC9, 0xFFCA, 0xFFCB, 0xFFCC, 0xFFCD, 0xFFCE, 0xFFCF,
    /* D */ 0xFFD0, 0xFFD1, 0xFFD2, 0xFFD3, 0xFFD4, 0xFFD5, 0xFFD6, 0xFFD7,
    0xFFD8, 0xFFD9, 0xFFDA, 0xFFDB, 0xFFDC, 0xFFDD, 0xFFDE, 0xFFDF,
    /* E */ 0xFFE0, 0xFFE1, 0xFFE2, 0xFFE3, 0xFFE4, 0xFFE5, 0xFFE6, 0xFFE7,
    0xFFE8, 0xFFE9, 0xFFEA, 0xFFEB, 0xFFEC, 0xFFED, 0xFFEE, 0xFFEF,
    /* F */ 0xFFF0, 0xFFF1, 0xFFF2, 0xFFF3, 0xFFF4, 0xFFF5, 0xFFF6, 0xFFF7,
    0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD, 0xFFFE, 0xFFFF,
};

static int hfs_unicode_compare_int(uint16_t endian, const hfs_uni_str * uni1, const hfs_uni_str * uni2)
{
    uint16_t c1, c2;
    uint16_t temp;
    uint16_t *lowerCaseTable;

    const uint8_t *str1 = uni1->unicode;
    const uint8_t *str2 = uni2->unicode;
    uint16_t length1 = tsk_getu16(endian, uni1->length);
    uint16_t length2 = tsk_getu16(endian, uni2->length);

    lowerCaseTable = gLowerCaseTable;

    while (1) {
        //  Set default values for c1, c2 in case there are no more valid chars
        c1 = 0;
        c2 = 0;
        //  Find next non-ignorable char from str1, or zero if no more
        while (length1 && c1 == 0) {
            c1 = tsk_getu16(endian, str1);
            str1 += 2;
            --length1;
            if ((temp = lowerCaseTable[c1 >> 8]) != 0)  //  is there a subtable
                //  for this upper byte?
                c1 = lowerCaseTable[temp + (c1 & 0x00FF)];      //  yes, so fold the char
        }
        //  Find next non-ignorable char from str2, or zero if no more
        while (length2 && c2 == 0) {
            c2 = tsk_getu16(endian, str2);
            str2 += 2;
            --length2;
            if ((temp = lowerCaseTable[c2 >> 8]) != 0)  //  is there a subtable
                //  for this upper byte?
                c2 = lowerCaseTable[temp + (c2 & 0x00FF)];      //  yes, so fold the char
        }
        if (c1 != c2)           //  found a difference, so stop looping
            break;
        if (c1 == 0)            //  did we reach the end of both strings at the same time?
            return 0;           //  yes, so strings are equal
    }
    if (c1 < c2)
        return -1;
    else
        return 1;
}

int hfs_unicode_compare(HFS_INFO * hfs, const hfs_uni_str * uni1, const hfs_uni_str * uni2)
{
    if (hfs->is_case_sensitive) {
        uint16_t l1, l2;
        const uint8_t *s1, *s2;
        uint16_t c1, c2;

        l1 = tsk_getu16(hfs->fs_info.endian, uni1->length);
        l2 = tsk_getu16(hfs->fs_info.endian, uni2->length);
        s1 = uni1->unicode;
        s2 = uni2->unicode;

        while (1) {
            if ((l1 == 0) && (l2 == 0))
                return 0;
            if (l1 == 0)
                return -1;
            if (l2 == 0)
                return 1;
            c1 = tsk_getu16(hfs->fs_info.endian, s1);
            c2 = tsk_getu16(hfs->fs_info.endian, s2);
            if (c1 < c2)
                return -1;
            if (c1 > c2)
                return 1;
            s1 += 2;
            s2 += 2;
            --l1;
            --l2;
        }
        return 0;
    }
    else
        return hfs_unicode_compare_int(hfs->fs_info.endian, uni1, uni2);
}

static TSK_INUM_T
hfs_lookup_hard_link(HFS_INFO * hfs, TSK_INUM_T linknum,
    unsigned char is_directory)
{
    char fBuff[30];
    TSK_FS_DIR *mdir;
    size_t indx;
    TSK_FS_INFO *fs = (TSK_FS_INFO *) hfs;

    memset(fBuff, 0, 30);

    if (is_directory) {

        tsk_take_lock(&(hfs->metadata_dir_cache_lock));
        if (hfs->dir_meta_dir == NULL) {
            hfs->dir_meta_dir =
                tsk_fs_dir_open_meta(fs, hfs->meta_dir_inum);
        }
        tsk_release_lock(&(hfs->metadata_dir_cache_lock));

        if (hfs->dir_meta_dir == NULL) {
            //error_returned
            //    ("hfs_lookup_hard_link: could not open the dir metadata directory");
            return 0;
        }
        else {
            mdir = hfs->dir_meta_dir;
        }
        snprintf(fBuff, 30, "dir_%" PRIuINUM, linknum);

    }
    else {

        tsk_take_lock(&(hfs->metadata_dir_cache_lock));
        if (hfs->meta_dir == NULL) {
            hfs->meta_dir = tsk_fs_dir_open_meta(fs, hfs->meta_inum);
        }
        tsk_release_lock(&(hfs->metadata_dir_cache_lock));

        if (hfs->meta_dir == NULL) {
            //error_returned
            //    ("hfs_lookup_hard_link: could not open file metadata directory");
            return 0;
        }
        else {
            mdir = hfs->meta_dir;
        }
        snprintf(fBuff, 30, "iNode%" PRIuINUM, linknum);
    }

    for (indx = 0; indx < tsk_fs_dir_getsize(mdir); indx++) {

        if ((mdir->names != NULL) && mdir->names[indx].name &&
            (fs->name_cmp(fs, mdir->names[indx].name, fBuff) == 0)) {
            // OK this is the one
            return mdir->names[indx].meta_addr;
        }
    }

    // OK, we did not find that linknum
    return 0;
}

TSK_INUM_T hfs_follow_hard_link(HFS_INFO * hfs, hfs_file * cat, unsigned char *is_error)
{

    TSK_FS_INFO *fs = (TSK_FS_INFO *) hfs;
    TSK_INUM_T cnid;
    time_t crtime;
    uint32_t file_type;
    uint32_t file_creator;

    *is_error = 0;              // default, not an error

    if (cat == NULL) {
        //error_detected(TSK_ERR_FS_ARG,
        //    "hfs_follow_hard_link: Pointer to Catalog entry (2nd arg) is null");
        return 0;
    }

    cnid = tsk_getu32(fs->endian, cat->std.cnid);

    if (cnid < HFS_FIRST_USER_CNID) {
        // Can't be a hard link.  And, cannot look up in Catalog file either!
        return cnid;
    }

    crtime =
        (time_t) hfs_convert_2_unix_time(tsk_getu32(fs->endian,
            cat->std.crtime));


    file_type = tsk_getu32(fs->endian, cat->std.u_info.file_type);
    file_creator = tsk_getu32(fs->endian, cat->std.u_info.file_cr);

    // Only proceed with the rest of this if the flags etc are right
    if (file_type == HFS_HARDLINK_FILE_TYPE
        && file_creator == HFS_HARDLINK_FILE_CREATOR) {

        // see if we have the HFS+ Private Data dir for file links;
        // if not, it can't be a hard link.  (We could warn the user, but
        // we also rely on this when finding the HFS+ Private Data dir in
        // the first place and we don't want a warning on every hfs_open.)
        if (hfs->meta_inum == 0)
            return cnid;

        // For this to work, we need the FS creation times.  Is at least one of these set?
        if ((!hfs->has_root_crtime) && (!hfs->has_meta_dir_crtime)
            && (!hfs->has_meta_crtime)) {
            //uint32_t linkNum =
            uint32_t linkNum = tsk_getu32(fs->endian, cat->std.perm.special.inum);
            if(linkNum)
            {
                // log error here or something
            }
            *is_error = 1;
            //if (tsk_verbose)
            //    tsk_fprintf(stderr,
            //        "WARNING: hfs_follow_hard_link: File system creation times are not set. "
            //        "Cannot test inode for hard link. File type and creator indicate that this"
            //        " is a hard link (file), with LINK ID = %" PRIu32 "\n",
            //        linkNum);
            return cnid;
        }

        if ((!hfs->has_root_crtime) || (!hfs->has_meta_crtime)) {
            //if (tsk_verbose)
            //    tsk_fprintf(stderr,
            //        "WARNING: hfs_follow_hard_link: Either the root folder or the"
            //        " file metadata folder is not accessible.  Testing this potential hard link"
            //        " may be impaired.\n");
        }

        // Now we need to check the creation time against the three FS creation times
        if ((hfs->has_meta_crtime && (crtime == hfs->meta_crtime)) ||
            (hfs->has_meta_dir_crtime && (crtime == hfs->metadir_crtime))
            || (hfs->has_root_crtime && (crtime == hfs->root_crtime))) {
            // OK, this is a hard link to a file.
            uint32_t linkNum =
                tsk_getu32(fs->endian, cat->std.perm.special.inum);

            TSK_INUM_T target_cnid;     //  This is the real CNID of the file.

            target_cnid = hfs_lookup_hard_link(hfs, linkNum, FALSE);

            if (target_cnid != 0) {
                // Succeeded in finding that target_cnid in the Catalog file
                return target_cnid;
            }
            else {
                // This should be a hard link, BUT...
                // Did not find the target_cnid in the Catalog file.
                //error_returned
                //    ("hfs_follow_hard_link: got an error looking up the target of a file link");
                *is_error = 2;
                return 0;
            }
        }

    }
    else if (file_type == HFS_LINKDIR_FILE_TYPE
        && file_creator == HFS_LINKDIR_FILE_CREATOR) {

        // see if we have the HFS+ Private Directory Data dir for links;
        // if not, it can't be a hard link.  (We could warn the user, but
        // we also rely on this when finding the HFS+ Private Directory Data dir in
        // the first place and we don't want a warning on every hfs_open.)
        if (hfs->meta_dir_inum == 0)
            return cnid;

        // For this to work, we need the FS creation times.  Is at least one of these set?
        if ((!hfs->has_root_crtime) && (!hfs->has_meta_dir_crtime)
            && (!hfs->has_meta_crtime)) {
            uint32_t linkNum =
                tsk_getu32(fs->endian, cat->std.perm.special.inum);
            if(linkNum)
            {
                // put some error here
            }
            *is_error = 1;

            //if (tsk_verbose)
            //    tsk_fprintf(stderr,
            //        "WARNING: hfs_follow_hard_link: File system creation times are not set. "
            //        "Cannot test inode for hard link. File type and creator indicate that this"
            //        " is a hard link (directory), with LINK ID = %" PRIu32
            //        "\n", linkNum);
            return cnid;
        }

        if ((!hfs->has_root_crtime) || (!hfs->has_meta_crtime)
            || (!hfs->has_meta_dir_crtime)) {
            //if (tsk_verbose)
            //    tsk_fprintf(stderr,
            //        "WARNING: hfs_follow_hard_link: Either the root folder or the"
            //        " file metadata folder or the directory metatdata folder is"
            //        " not accessible.  Testing this potential hard linked folder "
            //        "may be impaired.\n");
        }

        // Now we need to check the creation time against the three FS creation times
        if ((hfs->has_meta_crtime && (crtime == hfs->meta_crtime)) ||
            (hfs->has_meta_dir_crtime && (crtime == hfs->metadir_crtime))
            || (hfs->has_root_crtime && (crtime == hfs->root_crtime))) {
            // OK, this is a hard link to a directory.
            uint32_t linkNum =
                tsk_getu32(fs->endian, cat->std.perm.special.inum);

            TSK_INUM_T target_cnid;     //  This is the real CNID of the file.

            target_cnid = hfs_lookup_hard_link(hfs, linkNum, TRUE);

            if (target_cnid != 0) {
                // Succeeded in finding that target_cnid in the Catalog file
                return target_cnid;
            }
            else {
                // This should be a hard link, BUT...
                // Did not find the target_cnid in the Catalog file.
                //error_returned
                //    ("hfs_follow_hard_link: got an error looking up the target of a dir link");
                *is_error = 2;
                return 0;
            }
        }
    }

    // It cannot be a hard link (file or directory)
    return cnid;
}

uint32_t hfs_convert_2_unix_time(uint32_t hfsdate)
{
    if(hfsdate < NSEC_BTWN_1904_1970)
        return 0;
    return (uint32_t)(hfsdate - NSEC_BTWN_1904_1970);
}
