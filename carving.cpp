#include "carving.h"

// Copyright 2013-2020 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

void PopulateCarvedFiles(QString cfilestr)
{
    QModelIndexList indexlist = treenodemodel->match(treenodemodel->index(0, 11, QModelIndex()), Qt::DisplayRole, QVariant(cfilestr.split(".").first()), -1, Qt::MatchFlags(Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap));
    if(indexlist.count() == 0)
    {
	cfilestr = wombatvariable.tmpmntpath + "carved/" + cfilestr;
	QString tmpstr = "";
	QFile cfile(cfilestr);
	if(!cfile.isOpen())
	    cfile.open(QIODevice::ReadOnly | QIODevice::Text);
	if(cfile.isOpen())
	    tmpstr = cfile.readLine();
	cfile.close();
	QStringList slist = tmpstr.split(",");
	QList<QVariant> nodedata;
	nodedata.clear();
	nodedata << slist.at(0); // name
	nodedata << slist.at(3); // path
	nodedata << slist.at(8); // size
	nodedata << slist.at(6); // crtime
	nodedata << slist.at(4); // atime
	nodedata << slist.at(7); // mtime
	nodedata << slist.at(5); // ctime
	nodedata << slist.at(13); // hash
	nodedata << QString(slist.at(10)).split("/").first(); // category
	nodedata << QString(slist.at(10)).split("/").last(); // signature
	nodedata << slist.at(15); // tag
	nodedata << slist.at(12); // id
	mutex.lock();
	if(slist.at(12).split("-").count() == 2)
	    treenodemodel->AddNode(nodedata, QString(slist.at(12).split("-").first() + "-mc"), 15, 0);
	else
	    treenodemodel->AddNode(nodedata, QString(slist.at(12).split("-c").first() + "-" + slist.at(17)), 15, 0);
	mutex.unlock();
	listeditems.append(slist.at(12));
    }
}

void GetCarvers(QStringList& ctypelist, QStringList flist) 
{
    QString hpath = QDir::homePath();
    hpath += "/.local/share/wombatforensics/";
    QFile ctypes(hpath + "carvetypes");
    if(!ctypes.isOpen())
	ctypes.open(QIODevice::ReadOnly | QIODevice::Text);
    if(ctypes.isOpen())
    {
        QTextStream in(&ctypes);
        while(!in.atEnd())
        {
            QString tmpstr = in.readLine();
            for(int i=0; i < flist.count(); i++)
            {
                if(flist.at(i).contains(tmpstr.split(",").at(1)))
                    ctypelist.append(tmpstr);
            }
        }
        ctypes.close();
    }
}

void GetPartitionValues(qint64& partoffset, qint64& blocksize, qint64& partsize, QFile& rawfile, QString curpartid)
{
    QString estring = curpartid.split("-").first();
    QString vstring = curpartid.split("-").at(1);
    QString pstring = curpartid.split("-").at(2);
    // get offset and length to find within the mounted raw image file...
    QDir eviddir = QDir(wombatvariable.tmpmntpath);
    QStringList evidfiles = eviddir.entryList(QStringList(QString("*." + estring)), QDir::NoSymLinks | QDir::Dirs);
    QFile evidfile(wombatvariable.tmpmntpath + evidfiles.first() + "/stat");
    QStringList evidlist;
    evidlist.clear();
    if(!evidfile.isOpen())
        evidfile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(evidfile.isOpen())
    {
        evidlist = QString(evidfile.readLine()).split(",", Qt::SkipEmptyParts);
        evidfile.close();
    }
    QString rawevidencepath = wombatvariable.imgdatapath;
    if(TSK_IMG_TYPE_ISAFF((TSK_IMG_TYPE_ENUM)evidlist.at(0).toInt()))
        rawevidencepath += evidlist.at(3).split("/").last() + ".raw";
    else if(TSK_IMG_TYPE_ISEWF((TSK_IMG_TYPE_ENUM)evidlist.at(0).toInt()))
        rawevidencepath += evidlist.at(3).split("/").last() + "/ewf1";
    else if(TSK_IMG_TYPE_ISRAW((TSK_IMG_TYPE_ENUM)evidlist.at(0).toInt()))
    {
        QString imgext = evidlist.at(3).split("/").last().split("/").last();
        if(imgext.contains("000"))
            rawevidencepath += evidlist.at(3).split("/").last() + ".raw";
        else
            rawevidencepath = evidlist.at(3);
    }
    QString evidencename = evidfiles.at(0).split(".e").first();
    QString partstr = wombatvariable.tmpmntpath + evidencename + "." + estring + "/" + vstring + "/" + pstring + "/stat";
    QFile partfile(partstr);
    QStringList partlist;
    partlist.clear();
    if(!partfile.isOpen())
        partfile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(partfile.isOpen())
    {
        QString tmpstr = partfile.readLine();
        partlist = tmpstr.split(",");
        partfile.close();
    }
    rawfile.setFileName(rawevidencepath);
    if(!rawfile.isOpen())
        rawfile.open(QIODevice::ReadOnly);
    partoffset = partlist.at(4).toLongLong();
    blocksize = evidlist.at(2).toLongLong(); // SECTOR SIZE, RATHER THAN FS CLUSTER SIZE
    partsize = partlist.at(1).toLongLong() - partoffset;

}

void GetExistingCarvedFiles(QHash<int, QString>& headhash, qint64& blocksize)
{
    QDir cdir = QDir(wombatvariable.tmpmntpath + "carved/");
    QStringList cfiles = cdir.entryList(QStringList("e*-c*"), QDir::NoSymLinks | QDir::Files);
    if(!cfiles.isEmpty())
    {
	for(int j=0; j < cfiles.count(); j++)
	{
	    qint64 byteoffset = 0;
	    QFile cfile(wombatvariable.tmpmntpath + "carved/" + cfiles.at(j));
	    if(!cfile.isOpen())
		cfile.open(QIODevice::ReadOnly | QIODevice::Text);
	    if(cfile.isOpen())
		byteoffset = QString(cfile.readLine()).split(",").at(16).toLongLong();
	    qint64 curblock = byteoffset / blocksize;
	    headhash.insert(curblock, "");
	}
    }
}

void FirstCarve(qint64& blockcount, QStringList& ctypelist, QList<int>& blocklist, QHash<int, QString>& headhash, QFile& rawfile, qint64& blocksize, qint64& partoffset)
{
    for(int i=0; i < blockcount; i++)
    {
	if(!headhash.contains(i))
	{
	    for(int j=0; j < ctypelist.count(); j++)
	    {
		QString curheadnam = ctypelist.at(j).split(",").at(1);
		if(curheadnam.contains("JPEG") || curheadnam.contains("PNG") || curheadnam.contains("GIF") || curheadnam.contains("PDF"))
		    HeaderSearch(i, ctypelist.at(j), rawfile, blocksize, partoffset, blocklist, headhash);
        	else if(curheadnam.contains("MPEG"))
		    FooterSearch(i, ctypelist.at(j), rawfile, blocksize, partoffset, blocklist, headhash);
		else
		    HeaderSearch(i, ctypelist.at(j), rawfile, blocksize, partoffset, blocklist, headhash);
	    }
	}
    }
}

void SecondCarve(QList<int>& blocklist, QHash<int, QString>& headhash, qint64& blocksize, QFile& rawfile, qint64& partoffset, qint64& blockcount, QByteArray& footerarray, QString& curplist)
{
    for(int i=0; i < blocklist.count(); i++)
    {
	qint64 carvedstringsize = 0;
	int curblock = blocklist.at(i);
	qint64 byteoffset = 0;
	bool isvalidfile = false;
        QString curtypestr = headhash.value(blocklist.at(i));
        QString curheadnam = curtypestr.split(",").at(1);
        if(curheadnam.contains("JPEG") || curheadnam.contains("PNG") || curheadnam.contains("GIF") || curheadnam.contains("PDF"))
            HeaderFooterSearch(curtypestr, blocklist, i, blocksize, rawfile, partoffset, blockcount, footerarray, carvedstringsize);
        else if(curheadnam.contains("MPEG"))
            FooterHeaderSearch(curtypestr, blocklist, i, blocksize, rawfile, partoffset, blockcount, footerarray, carvedstringsize, byteoffset);
	else
	    HeaderFooterSearch(curtypestr, blocklist, i, blocksize, rawfile, partoffset, blockcount, footerarray, carvedstringsize);
	if(byteoffset != 0) // for reverse search, fix curblock so it uses header and not initial footer block which was found
	    curblock = byteoffset / blocksize;
	ValidateCarvedFile(isvalidfile, footerarray, curtypestr, carvedstringsize, curplist);
	WriteCarvedFile(curplist, carvedstringsize, blocksize, curblock, curtypestr, isvalidfile);
    }
}

void HeaderSearch(int& j, QString carvetype, QFile& rawfile, qint64& blocksize, qint64& partoffset, QList<int>& blocklist, QHash<int, QString>& headhash)
{
    QString curheadcat = carvetype.split(",").at(0);
    QString curheadnam = carvetype.split(",").at(1);
    QString curheadstr = carvetype.split(",").at(2);
    QString curfootstr = carvetype.split(",").at(3);
    QString curextstr = carvetype.split(",").at(4);
    QString curmaxsize = carvetype.split(",").at(5);
    int bytecount = curheadstr.count() / 2;
    if(curheadnam.contains("JPEG"))
        bytecount = 11;
    QByteArray headerarray;
    headerarray.clear();
    bool isseek = rawfile.seek(partoffset + (j * blocksize));
    if(isseek)
        headerarray = rawfile.read(bytecount);
    QString blockheader = QString::fromStdString(headerarray.toHex(0).toStdString()).toUpper();
    if(curheadnam.contains("JPEG"))
    {
        QString exifjpghdr = "FFD8FFE1????45786966";
        QString spifjpghdr = "FFD8FFE8????5350494646";
        QString jfifjpghdr = "FFD8FFE0????4A464946";
        if(blockheader.contains("FFD8FFE1") && blockheader.contains("45786966")) // EXIF JPEG
        {
            curheadnam = "EXIF JPEG";
            curheadstr = exifjpghdr;
        }
        else if(blockheader.contains("FFD8FFE8") && blockheader.contains("5350494646")) // SPIFF JPEG
        {
            curheadnam = "SPIFF JPEG";
            curheadstr = spifjpghdr;
        }
        else if(blockheader.contains("FFD8FFE0") && blockheader.contains("4A464946")) // JFIF JPEG
        {
            curheadnam = "JFIF JPEG";
            curheadstr = jfifjpghdr;
        }
        carvetype = curheadcat + "," + curheadnam + "," + curheadstr + "," + curfootstr + "," + curextstr + "," + curmaxsize;
    }
    // COMPARE BLOCK HEADER TO THE CURHEADSTR FOR MATCH...
    int headleft = curheadstr.indexOf("?");
    int headright = curheadstr.lastIndexOf("?");
    if(headleft == -1 && headright == -1) // header without ???'s
    {
        if(blockheader.startsWith(curheadstr))
        {
            blocklist.append(j);
            headhash.insert(j, carvetype);
        }
    }
    else // header with ???'s
    {
        if(blockheader.left(headleft).contains(curheadstr.left(headleft)) && blockheader.mid(headright+1).contains(curheadstr.mid(headright+1)))
        {
            blocklist.append(j);
            headhash.insert(j, carvetype);
        }
    }
}

void FooterSearch(int& j, QString carvetype, QFile& rawfile, qint64& blocksize, qint64& partoffset, QList<int>& blocklist, QHash<int, QString>& headhash)
{
    QString curheadcat = carvetype.split(",").at(0);
    QString curheadnam = carvetype.split(",").at(1);
    QString curheadstr = carvetype.split(",").at(2);
    QString curfootstr = carvetype.split(",").at(3);
    QString curextstr = carvetype.split(",").at(4);
    QString curmaxsize = carvetype.split(",").at(5);
    QByteArray footerarray;
    footerarray.clear();
    bool isseek = rawfile.seek(partoffset + (j * blocksize));
    if(isseek)
	footerarray = rawfile.read(blocksize);
    QString blockfooter = QString::fromStdString(footerarray.toHex(0).toStdString()).toUpper();
    if(curheadnam.contains("MPEG"))
    {
	QString mpg2ftr = "000001B9";
	QString mpgftr = "000001B7";
	if(blockfooter.contains(mpg2ftr)) // MPEG-2
	{
	    curheadnam = "MPEG-2";
	    curheadstr = "000001BA";
	    curfootstr = mpg2ftr;
	}
	else if(blockfooter.contains(mpgftr)) // MPEG-1
	{
	    curheadnam = "MPEG-1";
	    curheadstr = "000001B?";
	    curfootstr = mpg2ftr;
	}
	carvetype = curheadcat + "," + curheadnam + "," + curheadstr + "," + curfootstr + "," + curextstr + "," + curmaxsize;
    }
    if(blockfooter.contains(curfootstr))
    {
	blocklist.append(j);
	headhash.insert(j, carvetype);
    }
}

void HeaderFooterSearch(QString& carvetype, QList<int>& blocklist, int& j, qint64& blocksize, QFile& rawfile, qint64& partoffset, qint64& blockcount, QByteArray& footerarray, qint64& carvedstringsize)
{
    qint64 blockdifference = 0;
    qint64 curmaxsize = carvetype.split(",").at(5).toLongLong();
    qint64 arraysize = 0;
    QString curfooter = carvetype.split(",").at(3); // find footers
    if(j == (blocklist.count() - 1))
        blockdifference = (blockcount - blocklist.at(j)) * blocksize;
    else
        blockdifference = (blocklist.at(j+1) - blocklist.at(j)) * blocksize;
    if(curfooter.isEmpty() && j == (blocklist.count() - 1))
    {
        if(blockdifference < curmaxsize)
            arraysize = blockdifference;
        else
            arraysize = curmaxsize;
    }
    else
        arraysize = blockdifference;
    qint64 lastfooterpos = -1;
    if(!curfooter.isEmpty()) // if footer exists
    {
        bool isseek = rawfile.seek(partoffset + (blocklist.at(j) * blocksize));
        if(isseek)
            footerarray = rawfile.read(arraysize);
        QString footerstr = QString::fromStdString(footerarray.toHex().toStdString()).toUpper();

        lastfooterpos = footerstr.lastIndexOf(curfooter);
        if(lastfooterpos == -1) // no footer found, use full length
            carvedstringsize = arraysize;
        else // footer found, so use it
            carvedstringsize = lastfooterpos + curfooter.count();
    }
    else // no footer defined, just use arraysize as size
        carvedstringsize = arraysize;
}

void FooterHeaderSearch(QString& carvetype, QList<int>& blocklist, int& j, qint64& blocksize, QFile& rawfile, qint64& partoffset, qint64& blockcount, QByteArray& footerarray, qint64& carvedstringsize, qint64& byteoffset)
{
    qint64 blockdifference = 0;
    qint64 arraysize = 0;
    QString curheader = carvetype.split(",").at(2); // find headers
    qint64 seekstart = partoffset;
    if(j == 0)
	blockdifference = (blocklist.at(j) * blocksize) + blocksize;
    else
    {
	blockdifference = ((blocklist.at(j) - blocklist.at(j-1)) * blocksize) - blocksize;
	seekstart = blocklist.at(j-1) * blocksize + partoffset + blocksize;
    }
    arraysize = blockdifference;
    qint64 firstheaderpos = -1;
    bool isseek = rawfile.seek(seekstart);
    if(isseek)
	footerarray = rawfile.read(arraysize);
    QString headerstr = QString::fromStdString(footerarray.toHex().toStdString()).toUpper();
    firstheaderpos = headerstr.indexOf(curheader);
    byteoffset = seekstart + firstheaderpos / 2;
    if(firstheaderpos == -1) // no header found, use full length
    {
	carvedstringsize = arraysize;
    }
    else
	carvedstringsize = arraysize - firstheaderpos / 2;
}

void ValidateCarvedFile(bool& isvalidfile, QByteArray& footerarray, QString& carvetype, qint64& carvedstringsize, QString& curplist)
{
    QImage testimg;
    if(carvetype.split(",").at(1).contains("JPEG")) // validity check for jpeg
        isvalidfile = testimg.loadFromData(footerarray.left(carvedstringsize), "jpg");
    else if(carvetype.split(",").at(1).contains("PNG")) // validity check for png
        isvalidfile = testimg.loadFromData(footerarray.left(carvedstringsize), "png");
    else if(carvetype.split(",").at(1).contains("GIF")) // validity check for gif
        isvalidfile = testimg.loadFromData(footerarray.left(carvedstringsize), "gif");
    else if(carvetype.split(",").at(1).contains("PDF")) // validity check for pdf
    {
        Poppler::Document* tmpdoc = NULL;
        tmpdoc = Poppler::Document::loadFromData(footerarray.left(carvedstringsize));
        if(tmpdoc != NULL)
        {
            Poppler::Page* tmppage = NULL;
            tmppage = tmpdoc->page(0); // load initial page
            if(tmppage != NULL)
            {
                QImage curimage = tmppage->renderToImage();
                isvalidfile = curimage.isNull();
            }
            delete tmppage;
        }
        delete tmpdoc;
    }
    else if(carvetype.split(",").at(1).contains("MPEG"))
    {
        QByteArray tmparray = footerarray.left(carvedstringsize);

	QString estring = curplist.split("-").at(0);
	QString vstring = curplist.split("-").at(1);
	QString pstring = curplist.split("-").at(2);
        QString tmpfstr = wombatvariable.tmpfilepath + estring + "-" + vstring + "-" + pstring + "-c" + QString::number(carvedcount) + ".tmp";
        QFile tfile(tmpfstr);
        tfile.open(QIODevice::WriteOnly);
        QDataStream otbuf(&tfile);
        otbuf.writeRawData(tmparray, tmparray.count());
        tfile.close();
        VideoViewer* tmpvid = new VideoViewer();
        isvalidfile = tmpvid->LoadFile(tmpfstr);
        delete tmpvid;
        // NEED TO LOOK INTO POSSIBLY USING LIBMPEG2 TO VALIDATE.. OR JUST NOT VALIDATE AT ALL...
    }

}
void WriteCarvedFile(QString& curplist, qint64& carvedstringsize, qint64& blocksize, int& curblock, QString& curtypestr, bool& isvalidfile)
{
    QString parstr = curplist;
    QString vtype = "";
    if(isvalidfile)
	vtype = "pc";
    else
	vtype = "uc";
    parstr += vtype;
    // NEED TO FIX THE CARVEOFFSET FOR MPG SO IT STARTS AT HEADER AND NOT FOOTER...
    // DO STAT/TREENODE here and everything else everywhere else to make it work.
    QString cstr = QByteArray(QString("Carved" + QString::number(curblock) + "." + curtypestr.split(",").at(4).toLower()).toStdString().c_str()).toBase64() + ",5,0," + QByteArray(QString("0x" + QString::number(curblock*blocksize, 16)).toStdString().c_str()).toBase64() + ",0,0,0,0," + QString::number(carvedstringsize) + "," + QString::number(carvedcount) + "," + curtypestr.split(",").at(0) + "/" + curtypestr.split(",").at(1) + ",0," + curplist + "-c" + QString::number(carvedcount) + ",0,0,0," + QString::number(curblock*blocksize) + "," + vtype; //,addr,mime/cat,id,hash,deleted,bookmark,carveoffset,validtype ;
    QFile cfile(wombatvariable.tmpmntpath + "carved/" + curplist + "-c" + QString::number(carvedcount) + ".stat");
    if(!cfile.isOpen())
	cfile.open(QIODevice::WriteOnly | QIODevice::Text);
    if(cfile.isOpen())
    {
	cfile.write(cstr.toStdString().c_str());
	cfile.close();
    }
    QList<QVariant> nodedata;
    nodedata.clear();
    nodedata << QByteArray(QString("Carved" + QString::number(curblock) + "." + curtypestr.split(",").at(4).toLower()).toStdString().c_str()).toBase64() << QByteArray(QString("0x" + QString::number(curblock*blocksize, 16)).toStdString().c_str()).toBase64() << QString::number(carvedstringsize) << "0" << "0" << "0" << "0" << "0" << curtypestr.split(",").at(0) << curtypestr.split(",").at(1) << "" << curplist + "-c" + QString::number(carvedcount);
    listeditems.append(QString(curplist + "-c" + QString::number(carvedcount)));
    isignals->ProgUpd();
    isignals->CarveUpd(curplist, carvedcount);
    carvedcount++;
    filesfound++;
}

void GenerateCarving(QStringList plist, QStringList flist)
{
    // DETERMINE WHAT I AM CARVING FOR
    QStringList ctypelist;
    ctypelist.clear();
    GetCarvers(ctypelist, flist);
    // add current carving settings to log somehow...
    qInfo() << "Carving for:" << ctypelist;
    for(int i=0; i < plist.count(); i++) // for each partition start carving process...
    {
	qint64 partoffset = 0; // partition offset
	qint64 blocksize = 512; // sector size
	qint64 partsize = 0; // partition size
	QFile rawfile; // rawfile dd to pull content from
	GetPartitionValues(partoffset, blocksize, partsize, rawfile, plist.at(i));
	qint64 blockcount = partsize / blocksize;

	QHash<int, QString> headhash;
	headhash.clear();
	GetExistingCarvedFiles(headhash, blocksize);

	QList<int> blocklist;
	blocklist.clear();
	FirstCarve(blockcount, ctypelist, blocklist, headhash, rawfile, blocksize, partoffset);
	qInfo() << blocklist.count() << "Headers found. Starting Footer search...";
        QByteArray footerarray;
        footerarray.clear();
	QString curplist = plist.at(i);
        SecondCarve(blocklist, headhash, blocksize, rawfile, partoffset, blockcount, footerarray, curplist);
	if(rawfile.isOpen())
	    rawfile.close();
	QDir cdir = QDir(wombatvariable.tmpmntpath + "carved/");
	QStringList cfiles = cdir.entryList(QStringList(plist.at(i) + "-c*"), QDir::NoSymLinks | QDir::Files);
	if(!cfiles.isEmpty())
	{
	    QFuture<void> tmpfuture = QtConcurrent::map(cfiles, PopulateCarvedFiles);
	    tmpfuture.waitForFinished();
	}
    }
}
