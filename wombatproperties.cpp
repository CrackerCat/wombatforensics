#include "wombatproperties.h"

WombatProperties::WombatProperties(WombatVariable* wombatvarptr)
{
    wombatptr = wombatvarptr;
    affinfo = NULL;
    ewfinfo = NULL;
    ewfvalue = (uint8_t*)malloc(sizeof(uint8_t)*64);
    uvalue8bit = 0;
    value8bit = 0;
    value32bit = 0;
    value64bit = 0;
    size64bit = 0;
    ewferror = NULL;
    ffs = NULL;
    sb1 = NULL;
    sb2 = NULL;
    ext2fs = NULL;
    /*
    TSK_FS_FILE* tmpfile = NULL;
    ntfs_sb* ntfssb = NULL;
    FATXXFS_DENTRY* tmpfatdentry = NULL;
    FATXXFS_DENTRY* curentry = NULL;
    FATFS_INFO* fatfs = NULL;
    FATXXFS_SB* fatsb = NULL;
    const TSK_FS_ATTR*tmpattr;
    TSK_DADDR_T cursector = 0;
    TSK_DADDR_T endsector = 0;
    int8_t isallocsec = 0;
    TSK_INUM_T curinum = 0;
    FATFS_DENTRY* dentry = NULL;
    ISO_INFO* iso = NULL;
    iso9660_pvd_node* p = NULL;
    iso9660_svd_node* s = NULL;
    HFS_INFO* hfs = NULL;
    hfs_plus_vh* hsb = NULL;
    char fn[HFS_MAXNAMLEN + 1];
    HFS_ENTRY* hfsentry = NULL;
    hfs_btree_key_cat key;
    hfs_thread thread;
    hfs_file_folder record;
    TSK_OFF_T off;
    char* databuffer = NULL;
    ssize_t cnt;
    ssize_t bytesread = 0;
    int a;
    uint len = 0;
    char asc[512];
    char timebuf[128];
    */

}

QString WombatProperties::GetFileSystemLabel()
{
    return "";
}

QStringList WombatProperties::PopulateEvidenceImageProperties()
{
    propertylist.clear();
    propertylist << QString("File Format") << QString(tsk_img_type_todesc((TSK_IMG_TYPE_ENUM)wombatptr->evidenceobject.imageinfo->itype)) << QString("File Format the evidence data is stored in. Usually it is either a raw image (.dd/.001) or an embedded image (.E01/.AFF). A raw image contains only the data from the evidence. The embedded image contains other descriptive information from the acquisition.");
    propertylist << QString("Sector Size") << QString(QString::number(wombatptr->evidenceobject.imageinfo->sector_size) + " bytes") << QString("Sector size of the device. A Sector is a subdivision of a disk where data is stored. It is the smallest value used to divide the disk.");
    propertylist << QString("Sector Count") << QString(QString::number((int)((float)wombatptr->evidenceobject.imageinfo->size/(float)wombatptr->evidenceobject.imageinfo->sector_size)) + " sectors") << QString("The number of sectors in the disk.");
    propertylist << QString("Image Path") << QString::fromStdString(wombatptr->evidenceobject.fullpathvector[0]) << QString("Location where the evidence image is stored and read from.");
    if(TSK_IMG_TYPE_ISAFF(wombatptr->evidenceobject.imageinfo->itype)) // its AFF
    {
        affinfo = (IMG_AFF_INFO*)wombatptr->evidenceobject.imageinfo;
        propertylist << "MD5" << QString::fromStdString(GetSegmentValue(affinfo, AF_MD5)) << "The MD5 hash algorithm of the uncompressed image file, stored as a 128-bit value";
        propertylist << "Image GID" << QString::fromStdString(GetSegmentValue(affinfo, AF_IMAGE_GID)) << "A unique global identifier for the image";
        propertylist << "Device Model" << QString::fromStdString(GetSegmentValue(affinfo, AF_DEVICE_MODEL)) << "Acquired Drive Model number";
        propertylist << "Creator" << QString::fromStdString(GetSegmentValue(affinfo, AF_CREATOR)) << "Examiner who initiated the image acquisition";
        propertylist << "Case Number" << QString::fromStdString(GetSegmentValue(affinfo, AF_CASE_NUM)) << "The case number that the image is associated with";
        propertylist << "SHA1" << QString::fromStdString(GetSegmentValue(affinfo, AF_SHA1)) << "The SHA1 hash algorithm of the uncompressed image file, stored as a 160-bit value";
        propertylist << "Acquisition Date" << QString::fromStdString(GetSegmentValue(affinfo, AF_ACQUISITION_DATE)) << "The date the acquisition was made";
        propertylist << "Acquisition Notes" << QString::fromStdString(GetSegmentValue(affinfo, AF_ACQUISITION_NOTES)) << "Notes regading the acquisition";
        propertylist << "Acquisition Device" << QString::fromStdString(GetSegmentValue(affinfo, AF_ACQUISITION_DEVICE)) << "The device used to acquire the information";
        propertylist << "AFFLib Version" << QString::fromStdString(GetSegmentValue(affinfo, AF_AFFLIB_VERSION)) << "Verion of the AFFLib Library used";
        propertylist << "Device Manufacturer" << QString::fromStdString(GetSegmentValue(affinfo, AF_DEVICE_MANUFACTURER)) << "Maker of the drive";
        propertylist << "Device Serial Number" << QString::fromStdString(GetSegmentValue(affinfo, AF_DEVICE_SN)) << "Serial number of the drive";
    }
    else if(TSK_IMG_TYPE_ISEWF(wombatptr->evidenceobject.imageinfo->itype)) // its EWF
    {
        ewfinfo = (IMG_EWF_INFO*)wombatptr->evidenceobject.imageinfo;
        if(libewf_handle_get_utf8_header_value_case_number(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Case Number" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "The case number the image is associated";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_description(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Description" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Description of the acquisition and or evidence item";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_examiner_name(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Examiner Name" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Name of the examiner who acquired the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_evidence_number(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Evidence Number" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Unique number identifying the evidence item";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_notes(ewfinfo->handle, ewfvalue, 255, &ewferror) == 1)
            propertylist << "Notes" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Any notes related to the acquisition";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_acquiry_date(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Acquisition Date" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Date the acquisition was made";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_system_date(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "System Date" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Date for the system acquiring the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_acquiry_operating_system(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Aquisition OS" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Operating System acquiring the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_acquiry_software_version(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Software Version Used" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Version of the software used to acquire the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_password(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Password" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Password to protect the image";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_model(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Model" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Model of the drive acquired";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_header_value_serial_number(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "Serial Number" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "Serial number of the drive acquired";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_sectors_per_chunk(ewfinfo->handle, &value32bit, &ewferror) == 1)
            propertylist << QString("Sectors Per Chunk") << QString::number(value32bit) << "Number of sectors in a image evidence chunk";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_format(ewfinfo->handle, &uvalue8bit, &ewferror) == 1)
        {
            propertylist << QString("File Format");
            switch(uvalue8bit)
            {
                case LIBEWF_FORMAT_EWF:
                    propertylist << QString("Original EWF") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_SMART:
                    propertylist << QString("SMART") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_FTK:
                    propertylist << QString("FTK Imager") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE1:
                    propertylist << QString("EnCase 1") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE2:
                    propertylist << QString("EnCase 2") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE3:
                    propertylist << QString("EnCase 3") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE4:
                    propertylist << QString("EnCase 4") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE5:
                    propertylist << QString("EnCase 5") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_ENCASE6:
                    propertylist << QString("EnCase 6") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_LINEN5:
                    propertylist << QString("Linen 5") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_LINEN6:
                    propertylist << QString("Linen 6") << "Format used to store the evidence image";
                    break;
                case LIBEWF_FORMAT_EWFX:
                    propertylist << QString("EWFX (extended ewf)") << QString("Extended EWF Format used to store the evidence image");
                    break;
                case LIBEWF_FORMAT_LOGICAL_ENCASE5:
                    propertylist << QString("LEF EnCase 5") << QString("Logical Evidence File EnCase 5 Format used to store the evidence image");
                    break;
                case LIBEWF_FORMAT_LOGICAL_ENCASE6:
                    propertylist << QString("LEF EnCase 6") << QString("Logical Evidence File EnCase 6 Format used to store the evidence image");
                    break;
                case LIBEWF_FORMAT_UNKNOWN:
                    propertylist << QString("Unknown Format") << "Format used to store the evidence image";
                    break;
            }
        }
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_error_granularity(ewfinfo->handle, &value32bit, &ewferror) == 1)
            propertylist << QString("Error Granularity") << QString::number(value32bit) << "Error block size";
        else
            libewf_error_fprint(ewferror, stdout);
        propertylist << "Compression Method" << "Deflate" << "Method used to compress the image";
        if(libewf_handle_get_compression_values(ewfinfo->handle, &value8bit, &uvalue8bit, &ewferror) == 1)
        {
            propertylist << "Compression Level";
            if(value8bit == LIBEWF_COMPRESSION_NONE)
                propertylist << "No Compression";
            else if(value8bit == LIBEWF_COMPRESSION_FAST)
                propertylist << "Good (Fast) Compression";
            else if(value8bit == LIBEWF_COMPRESSION_BEST)
                propertylist << "Best Compression";
            else
                propertylist << "Unknown Compression";
            propertylist << "The more compression, the slower the acquisition but the smaller the file size";
        }
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_media_type(ewfinfo->handle, &uvalue8bit, &ewferror) == 1)
        {
            propertylist << "Media Type";
            if(uvalue8bit == LIBEWF_MEDIA_TYPE_REMOVABLE)
                propertylist << "Removable Disk";
            else if(uvalue8bit == LIBEWF_MEDIA_TYPE_FIXED)
                propertylist << "Fixed Disk";
            else if(uvalue8bit == LIBEWF_MEDIA_TYPE_SINGLE_FILES)
                propertylist << "Single Files";
            else if(uvalue8bit == LIBEWF_MEDIA_TYPE_OPTICAL)
                propertylist << "Optical Disk (CD/DVD/BD)";
            else if(uvalue8bit == LIBEWF_MEDIA_TYPE_MEMORY)
                propertylist << "Memory (RAM)";
            else
                propertylist << "Unknown";
            propertylist << "Media type of the original evidence";
        }
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_media_flags(ewfinfo->handle, &uvalue8bit, &ewferror) == 1)
        {
            if(uvalue8bit == LIBEWF_MEDIA_FLAG_PHYSICAL)
                propertylist << "Media Flag" << "Physical" << "Directly connected disk";
            if(uvalue8bit == LIBEWF_MEDIA_FLAG_FASTBLOC)
                propertylist << "Media Flag" << "Fastbloc Write Blocked" << "Write blocked disk using Fastbloc";
            if(uvalue8bit == LIBEWF_MEDIA_FLAG_TABLEAU)
                propertylist << "Media Flag" << "Tableau Write Blocked" << "Write blocked disk using Tableau";
        }
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_bytes_per_sector(ewfinfo->handle, &value32bit, &ewferror) == 1)
            propertylist << "Bytes Per Sector" << QString::number(value32bit) << "Number of bytes in a sector";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_number_of_sectors(ewfinfo->handle, &value64bit, &ewferror) == 1)
            propertylist << "Number of Sectors" << QString::number(value64bit) << "Number of total sectors in the original media";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_chunk_size(ewfinfo->handle, &value32bit, &ewferror) == 1)
            propertylist << "Chunk Size" << QString::number(value32bit) << "The size of an image chunk";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_media_size(ewfinfo->handle, &size64bit, &ewferror) == 1)
            propertylist << "Media Size" << QString::number(size64bit) << "The size of the media";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_hash_value_md5(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "MD5" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "The MD5 hash algorithm of the uncompressed image stored as a 128-bit value";
        else
            libewf_error_fprint(ewferror, stdout);
        if(libewf_handle_get_utf8_hash_value_sha1(ewfinfo->handle, ewfvalue, 64, &ewferror) == 1)
            propertylist << "SHA1" << QString::fromUtf8(reinterpret_cast<char*>(ewfvalue)) << "The SHA1 hash algorithm of the uncompressed image stored as a 160-bit value";
        else
            libewf_error_fprint(ewferror, stdout);
        free(ewfvalue);
    }
    else if(TSK_IMG_TYPE_ISRAW(wombatptr->evidenceobject.imageinfo->itype)) // is raw
    {
        // nothing i want to add for raw right now.
    }
    else // not supported...
    {
        // log error about unsupported image type.
    }
    return propertylist;
}

QStringList WombatProperties::PopulatePartitionProperties()
{
    return QStringList("");
}

QStringList WombatProperties::PopulateFileSystemProperties(TSK_FS_INFO* curfsinfo)
{
    if(curfsinfo->ftype == TSK_FS_TYPE_EXT2 || curfsinfo->ftype == TSK_FS_TYPE_EXT3 || curfsinfo->ftype == TSK_FS_TYPE_EXT4 || curfsinfo->ftype == TSK_FS_TYPE_EXT_DETECT)
    {
        ext2fs = (EXT2FS_INFO*)curfsinfo;
        propertylist << "File System Type";
        if(curfsinfo->ftype == TSK_FS_TYPE_EXT2)
            propertylist << "ext2";
        else if(curfsinfo->ftype == TSK_FS_TYPE_EXT3)
            propertylist << "ext3";
        else if(curfsinfo->ftype == TSK_FS_TYPE_EXT4)
            propertylist << "ext4";
        else
            propertylist << "ext2";
        propertylist << "";
        propertylist << "Inode Count" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_inodes_count)) << "Number of Inodes in the file system (0-3)";
        propertylist << "Block Count" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_inodes_count)) << "Number of Blocks in the file system (4-7)";
        propertylist << "Reserved Blocks" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_r_blocks_count)) << "Number of blocks reserved to prevent the file system from filling up (8-11)";
        propertylist << "Unallocated Blocks" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_free_blocks_count)) << "Number of unallocated blocks (12-15)";
        propertylist << "Unallocated Inodes" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_free_inode_count)) << "Number of unnalocated inodes (16-19)";
        propertylist << "First Data Block" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_first_data_block)) << "Block where block group 0 starts (20-23)";
        propertylist << "Log Block Size" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_log_block_size)) << "Block size saved as the number of places to shift 1,024 to the left (24-27)";
        propertylist << "Log Fragment Size" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_log_frag_size)) << "Fragment size saved as the number of bits to shift 1,024 to the left (28-31)";
        propertylist << "Blocks per Group" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_blocks_per_group)) << "Number of blocks in each block group (32-35)";
        propertylist << "Fragments per Group" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_frags_per_group)) << "Number of fragments in each block group (36-39)";
        propertylist << "Inodes per Group" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_inodes_per_group)) << "Number of inodes in each block group (40-43)";
        sprintf(asc, "%s", (tsk_getu32(curfsinfo->endian, ext2fs->fs->s_mtime) > 0) ? tsk_fs_time_to_str(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_mtime), timebuf) : "empty");
        propertylist << "Last Mount Time" << QString::fromStdString(string(asc)) << "Last time the file system was mounted (44-47)";
        sprintf(asc, "%s", (tsk_getu32(curfsinfo->endian, ext2fs->fs->s_wtime) > 0) ? tsk_fs_time_to_str(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_wtime), timebuf) : "empty");
        propertylist << "Last Written Time" << QString::fromStdString(string(asc)) << "Last time data was written to the file system (48-51)";
        propertylist << "Current Mount Count" << QString::number(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_mnt_count)) << "Number of times the file system has been mounted (52-53)";
        propertylist << "Maximum Mount Count" << QString::number(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_max_mnt_count)) << "Maximum number of times the file system can be mounted (54-55)";
        propertylist << "Signature" << QString::number(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_magic)) << "File system signature value should be 0xef53 (56-57)";
        propertylist << "File System State";
        if(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_state) & EXT2FS_STATE_VALID)
            propertylist << "Unmounted properly";
        else if(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_state) & EXT2FS_STATE_ERROR)
            propertylist << "Errors Detected";
        else
            propertylist << "Orphan inodes were being recovered";
        propertylist << "State of the file system when it was last shut down (58-59)";
        propertylist << "Error Handling Method";
        if(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_errors) == 1)
            propertylist << "Continue";
        else if(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_errors) == 2)
            propertylist << "Remount as Read-Only";
        else
            propertylist << "Panic";
        propertylist << "Identifies what the OS should do when it encounters a file system error (60-61)";
        propertylist << "Minor Version" << QString::number(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_minor_rev_level)) << "Minor Revision Level (62-63)";
        sprintf(asc, "%s", (tsk_getu32(curfsinfo->endian, ext2fs->fs->s_lastcheck) > 0) ? tsk_fs_time_to_str(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_lastcheck), timebuf) : "empty");
        propertylist << "Last Checked" << QString::fromStdString(string(asc)) << "Last time the consistency of the file system was checked (64-67)";
        propertylist << "Interval b/w Checks" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_checkinterval)) << "Interval between forced consistency checks (68-71)";
        propertylist << "Creator OS";
        switch(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_creator_os))
        {
            case EXT2FS_OS_LINUX:
                propertylist << "Linux";
                break;
            case EXT2FS_OS_HURD:
                propertylist << "HURD";
                break;
            case EXT2FS_OS_MASIX:
                propertylist << "MASIX";
                break;
            case EXT2FS_OS_FREEBSD:
                propertylist << "FreeBSD";
                break;
            case EXT2FS_OS_LITES:
                propertylist << "LITES";
                break;
            default:
                propertylist << "Unknown";
                break;
        }
        propertylist << "OS that might have created the file system (72-75)";
        propertylist << "Major Version";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_rev_level) == EXT2FS_REV_ORIG)
            propertylist << "Static Structure";
        else
            propertylist << "Dynamic Structure";
        propertylist << "If the version is not set to dynamic, the values from bytes 84 and up might not be accurate (76-79)";
        propertylist << "UID to Use Reserved Blocks" << QString::number(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_def_resuid)) << "User ID that can use reserved blocks (80-81)";
        propertylist << "GID to Use Reserved Blocks" << QString::number(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_def_resgid)) << "Group ID that can use reserved blocks (82-83)";
        propertylist << "First Non-Reserved Inode" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_first_ino)) << "First non-reserved inode in the file system (84-87)";
        propertylist << "Inode Structure Size" << QString::number(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_inode_size)) << "Size of each inode structure (88-89)";
        propertylist << "Block Group for SuperBlock" << QString::number(tsk_getu16(curfsinfo->endian, ext2fs->fs->s_block_group_nr)) << "Superblock is part of the block group (if backup copy) (90-91)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_compat) & EXT2FS_FEATURE_COMPAT_DIR_PREALLOC)
            propertylist << "Compatible Feature" << "Directory Pre-allocation" << "Pre-allocate directory blocks to reduce fragmentation. The OS can mount the file system as normal if it does not support a compatible feature (92-95)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_compat) & EXT2FS_FEATURE_COMPAT_IMAGIC_INODES)
            propertylist << "Compatible Feature" << "IMagic Inodes" << "AFS server inodes exist (92-95)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_compat) & EXT2FS_FEATURE_COMPAT_HAS_JOURNAL)
            propertylist << "Compatible Feature" << "Journal" << "File System has a journal (92-95)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_compat) & EXT2FS_FEATURE_COMPAT_EXT_ATTR)
            propertylist << "Compatible Feature" << "Extended Attributes" << "Inodes have extended attributes (92-95)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_compat) & EXT2FS_FEATURE_COMPAT_RESIZE_INO)
            propertylist << "Compatible Feature" << "Resizable File System" << "File system can resize itself for larger aptitions (92-95)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_compat) & EXT2FS_FEATURE_COMPAT_DIR_INDEX)
            propertylist << "Compatible Feature" << "Directory Index" << "Directories use hash index (92-95)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_COMPRESSION)
            propertylist << "Incompatible Feature" << "Compression" << " The OS should not mount the file system if it does not support this incompatible feature (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_FILETYPE)
            propertylist << "Incompatible Feature" << "Filetype" << "Directory entries contain a file type field (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_RECOVER)
            propertylist << "Incompatible Feature" << "Needs Recovery" << "The file systems needs to be recovered (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_JOURNAL_DEV)
            propertylist << "Incompatible Feature" << "Journal Device" << "The file system uses a journal device (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_META_BG)
            propertylist << "Incompatible Feature" << "Meta Block Groups" << "The file system has meta block groups (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_EXTENTS)
            propertylist << "Incompatible Feature" << "Extents" << "The file system uses extents (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_64BIT)
            propertylist << "Incompatible Feature" << "64-bit" << "The file system is 64-bit capable (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_MMP)
            propertylist << "Incompatible Feature" << "Multiple Mount Protection" << "The OS should not mount the file system if it does not support this incompatible feature (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_FLEX_BG)
            propertylist << "Incompatible Feature" << "Flexible Block Groups" << "The OS should not mount the file system if it does not support this incompatible feature (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_EA_INODE)
            propertylist << "Incompatible Feature" << "Extended Attributes" << "The file system supports extended attributes (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_incompat) & EXT2FS_FEATURE_INCOMPAT_DIRDATA)
            propertylist << "Incompatible Feature" << "Directory Entry Data" << "The OS should not mount the file system if it does not support this incompatible feature (96-99)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_ro_compat) & EXT2FS_FEATURE_RO_COMPAT_SPARSE_SUPER)
            propertylist << "Read only Feature" << "Sparse Super" << "Sparse superblocks and goup descriptor tables. The OS should mount the file system as read only if it does not support this read only feature (100-103)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_ro_compat) & EXT2FS_FEATURE_RO_COMPAT_LARGE_FILE)
            propertylist << "Read only Feature" << "Large File" << "The OS should mount the file system as read only if it does not support this read only feature (100-103)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_ro_compat) & EXT2FS_FEATURE_RO_COMPAT_HUGE_FILE)
            propertylist << "Read only Feature" << "Huge File" << "The OS should mount the file system as read only if it does not support this read only feature (100-103)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_ro_compat) & EXT2FS_FEATURE_RO_COMPAT_BTREE_DIR)
            propertylist << "Read only Feature" << "BTree Directory" << "The OS should mount the file system as read only if it does not support this read only feature (100-103)";
        if(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_feature_ro_compat) & EXT2FS_FEATURE_RO_COMPAT_EXTRA_ISIZE)
            propertylist << "Read only Feature" << "Extra Inode Size" << "The OS should mount the file system as read only if it does not support this read only feature (100-103)";
        sprintf(asc, "%" PRIx64 "%" PRIx64 "", tsk_getu64(curfsinfo->endian, &(ext2fs->fs)->s_uuid[8]), tsk_getu64(curfsinfo->endian, &(ext2fs->fs)->s_uuid[0]));
        propertylist << "File System ID" << QString::fromStdString(string(asc)) << "File system ID. Found in the superblock at bytes (104-119)"; 
        propertylist << "File System Label" << QString::fromStdString(string(ext2fs->fs->s_volume_name)) << "File System Label. (120-135)"; 
        propertylist << "Last Mounted Path" << QString::fromStdString(string(ext2fs->fs->s_last_mounted)) << "Path where the file system was last mounted (136-199)";
        propertylist << "Algorithm Usage Bitmap" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_algorithm_usage_bitmap)) << "(200-203)";
        propertylist << "Blocks Preallocated for Files" << QString::number(ext2fs->fs->s_prealloc_blocks) << "Number of blocks to preallocate for files (204-204)";
        propertylist << "Blocks Preallocated for Directories" << QString::number(ext2fs->fs->s_prealloc_dir_blocks) << "Number of blocks to preallocate for directories (205-205)";
        propertylist << "Unused" << "Unused" << "Unused bytes (206-207)";
        sprintf(asc, "%" PRIx64 "%" PRIx64 "", tsk_getu64(curfsinfo->endian, &(ext2fs->fs)->s_journal_uuid[8]), tsk_getu64(curfsinfo->endian, &(ext2fs->fs)->s_journal_uuid[0]));
        propertylist << "Journal ID" << QString::fromStdString(string(asc)) << "Journal ID (208-223)";
        propertylist << "Journal Inode" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_journal_inum)) << "Journal Inode (224-227)";
        propertylist << "Journal Device" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_journal_dev)) << "Journal device (228-231)";
        propertylist << "Head of Oprhan Inode List" << QString::number(tsk_getu32(curfsinfo->endian, ext2fs->fs->s_last_orphan)) << "Head of orphan inode list. (232-235)";
        propertylist << "Unused" << "Unused" << "Unused (236-1023)";
    }
    else if(curfsinfo->ftype == TSK_FS_TYPE_FFS1 || curfsinfo->ftype == TSK_FS_TYPE_FFS1B || curfsinfo->ftype == TSK_FS_TYPE_FFS2 || TSK_FS_TYPE_FFS_DETECT)
    {
        ffs = (FFS_INFO*)curfsinfo;
        sb1 = ffs->fs.sb1;
        sb2 = ffs->fs.sb2;
        propertylist << "File System Type";
        if(curfsinfo->ftype == TSK_FS_TYPE_FFS1 || curfsinfo->ftype == TSK_FS_TYPE_FFS1B)
            propertylist << "UFS 1";
        else
            propertylist << "UFS 2";
        if(curfsinfo->ftype == TSK_FS_TYPE_FFS1 || curfsinfo->ftype == TSK_FS_TYPE_FFS1B)
        {
            propertylist << "";
            propertylist << "Unused" << "Unused" << "Unused (0-7)";
            propertylist << "Backup Superblock Offset" << QString::number(tsk_gets32(curfsinfo->endian, sb1->sb_off)) << "Offset to backup superblock in cylinder group relative to a \"base\" (8-11)";
            propertylist << "Group Descriptor Offset" << QString::number(tsk_gets32(curfsinfo->endian, sb1->gd_off)) << "Offset to group descriptor in cylinder group relative to a \"base\" (12-15)";
            propertylist << "Inode Table Offset" << QString::number(tsk_gets32(curfsinfo->endian, sb1->ino_off)) << "Offset to inode table in cylinder group relative to a \"base\" (16-19)";
            propertylist << "Data Block Offset" << QString::number(tsk_gets32(curfsinfo->endian, sb1->dat_off)) << "Offset to data blocks in cylinder group relative to a \"base\" (20-23)";
            propertylist << "Delta Value for Staggering Offset" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cg_delta)) << "Delta value for caluclating the staggering offset in cylinder group (24-27)";
            propertylist << "Mask for Staggering Offset" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cg_cyc_mask)) << "Mask for calculating the staggering offset (cycle value) in cylinder group (28-31)";
            sprintf(asc, "%s", (tsk_getu32(curfsinfo->endian, sb1->wtime) > 0) ? tsk_fs_time_to_str(tsk_getu32(curfsinfo->endian, sb1->wtime), timebuf) : "empty");
            propertylist << "Last Written Time" << QString::fromStdString(string(asc)) << "Last time data was written to the file system (32-35)";
            propertylist << "Number of Fragments" << QString::number(tsk_gets32(curfsinfo->endian, sb1->frag_num)) << "Number of fragments in the file system (36-39)";
            propertylist << "Number of Data Fragments" << QString::number(tsk_gets32(curfsinfo->endian, sb1->data_frag_num)) << "Number of fragments in the file system that can store file data (40-43)";
            propertylist << "Number of Cylinder Groups" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cg_num)) << "Number of cylinder groups in the file system (44-47)";
            propertylist << "Block Byte Size" << QString::number(tsk_gets32(curfsinfo->endian, sb1->bsize_b)) << "Size of a block in bytes (48-51)";
            propertylist << "Fragment Size" << QString::number(tsk_gets32(curfsinfo->endian, sb1->fsize_b)) << "Size of a fragment in bytes (52-55)";
            propertylist << "Block Framgent Size" << QString::number(tsk_gets32(curfsinfo->endian, sb1->bsize_frag)) << "Size of a block in fragments (56-59)";
            propertylist << "Non-Essential Values" << "Non-Essential Values" << "Non-Essential Values (60-95)";
            propertylist << "Number of Bits Convert Block to Fragment" << QString::number(tsk_gets32(curfsinfo->endian, sb1->fs_fragshift)) << "Number of bits to convert between a block address and a fragment address (96-99)";
            propertylist << "Non-Essential Values" << "Non-Essential Values" << "Non-Essential Values (100-119)";
            propertylist << "Inodes Per Block" << QString::number(tsk_gets32(curfsinfo->endian, sb1->fs_inopb)) << "Number of inodes per block in the inode table (120-123)";
            propertylist << "Non-Essential Values" << "Non-Essential Values" << "Non-Essential Values (124-143)";
            sprintf(asc, "%" PRIx32 "%" PRIx32 "", tsk_getu32(curfsinfo->endian, &sb1->fs_id[4]), tsk_getu32(curfsinfo->endian, &sb1->fs_id[0]));
            propertylist << "File System ID" << QString::fromStdString(string(asc)) << "File system ID (144-151)";
            propertylist << "Cylinder Group Fragment Address" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cg_saddr)) << "Fragment address of the cylinder group summary area (152-155)";
            propertylist << "Cylinder Group Summary Area Byte Size" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cg_ssize_b)) << "Size of the cylinder group summary area in bytes (156-159)";
            propertylist << "Cylinder Group Descriptor Byte Size" << QString::number(tsk_gets32(curfsinfo->endian, sb1->fs_cgsize)) << "Size of the cylinder group descriptor in bytes (160-163)";
            propertylist << "Non-Essential Values" << "Non-Essential Values" << "Non-Essential Values (164-175)";
            propertylist << "Cylinders in File System" << QString::number(tsk_gets32(curfsinfo->endian, sb1->fs_ncyl)) << "Number of cylinders in the file system (176-179)";
            propertylist << "Cylinders per Cylinder Group" << QString::number(tsk_gets32(curfsinfo->endian, sb1->fs_cpg)) << "Number of cylinders in a cylinder group (180-183)";
            propertylist << "Inodes per Cylinder Group" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cg_inode_num)) << "Number of inodes in a cylinder group (184-187)";
            propertylist << "Fragments per Cylinder Group" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cg_frag_num)) << "Number of fragments in a cylinder group (188-191)";
            propertylist << "Number of Directories" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cstotal.dir_num)) << "Number of directories (192-195)";
            propertylist << "Number of Free Blocks" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cstotal.blk_free)) << "Number of free blocks (196-199)";
            propertylist << "Number of Free Inodes" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cstotal.ino_free)) << "Number of free inodes (200-203)";
            propertylist << "Number of Free Fragments" << QString::number(tsk_gets32(curfsinfo->endian, sb1->cstotal.frag_free)) << "Number of free fragments (204-207)";
            propertylist << "Super Block Modified Flag" << QString::number(sb1->fs_fmod) << "Super Block Modified Flag (208-208)";
            propertylist << "Clean File System Flag" << QString::number(sb1->fs_clean) << "File system was clean when it was mounted (209-209)";
            propertylist << "Read Only File System Flag" << QString::number(sb1->fs_ronly) << "File system was mounted read only (210-210)";
            if(sb1->fs_flags & FFS_SB_FLAG_UNCLEAN)
                propertylist << "General Flags" << "Unclean" << "Set when the file system is mounted (211-211)";
            if(sb1->fs_flags & FFS_SB_FLAG_SOFTDEP)
                propertylist << "General Flags" << "Soft Dependencies" << "Soft dependencies are being used (211-211)";
            if(sb1->fs_flags & FFS_SB_FLAG_NEEDFSCK)
                propertylist << "General Flags" << "Needs Check" << "Needs consistency check next time the file system is mounted (211-211)";
            if(sb1->fs_flags & FFS_SB_FLAG_INDEXDIR)
                propertylist << "General Flags" << "Index Directories" << "Directories are indexed using a hashtree or B-Tree (211-211)";
            if(sb1->fs_flags & FFS_SB_FLAG_ACL)
                propertylist << "General Flags" << "Access Control Lists" << "Access control lists are being used (211-211)";
            if(sb1->fs_flags & FFS_SB_FLAG_MULTILABEL)
                propertylist << "General Flags" << "TrustedBSD MAC Multi-Label" << "TrustedBSD Mandatory Access Control multi-labels are being used (211-211)";
            if(sb1->fs_flags & FFS_SB_FLAG_UPDATED)
                propertylist << "General Flags" << "Updated Flag Location" << "Flags have been moved (211-211)";
            propertylist << "Last Mount Point" << QString::fromStdString(string(reinterpret_cast<char*>(sb1->last_mnt))) << "Last mount point (212-723)";
            propertylist << "Non-Essential Values" << "Non-Essential Values" << "Non-Essential Values (724-1371)";
            propertylist << "Signature" << QString::number(tsk_gets32(curfsinfo->endian, sb1->magic)) << "File System signature value should be 0x011954 (1372-1375)";
        }
        else // FFS2
        {
        }
    }
    return propertylist;
}

QStringList WombatProperties::PopulateFileProperties()
{
    return QStringList("");
}
// REFERENCE INFORMATION
    /*
    switch(curfsinfo->ftype)
    {
        case TSK_FS_TYPE_NTFS:
            len = roundup(sizeof(ntfs_sb), curfsinfo->img_info->sector_size);
            ntfssb = tsk_malloc(len);
            cnt = tsk_fs_read(curfsinfo, (TSK_OFF_T) 0, (char*) ntfssb, len);
            // will come back to NTFS.
            break;
        case TSK_FS_TYPE_EXFAT:


    }
        case TSK_FS_TYPE_NTFS:
                //NTFS_INFO* tmpinfo = (NTFS_INFO*)tmpfsinfo;
                if((tmpfile = tsk_fs_file_open_meta(tmpfsinfo, NULL, NTFS_MFT_VOL)) == NULL)
                {
                    // log error here...
                }
                tmpattr = tsk_fs_attrlist_get(tmpfile->meta->attr, TSK_FS_ATTR_TYPE_NTFS_VNAME);
                //tmpattr = tsk_fs_attrlist_get(tmpfile->meta->attr, NTFS_ATYPE_VNAME);
                if(!tmpattr)
                {
                    // log error here...
                }
                if((tmpattr->flags & TSK_FS_ATTR_RES) && (tmpattr->size))
                {
                    UTF16* name16 = (UTF16*) tmpattr->rd.buf;
                    UTF8* name8 = (UTF8*) asc;
                    int retval;
                    retval = tsk_UTF16toUTF8(tmpfsinfo->endian, (const UTF16**)&name16, (UTF16*) ((uintptr_t) name16 + (int) tmpattr->size), &name8, (UTF8*) ((uintptr_t)name8 + sizeof(asc)), TSKlenientConversion);
                    if(retval != TSKconversionOK)
                    {
                        // log error here                                
                        *name8 = '\0';
                    }
                    else if((uintptr_t) name8 >= (uintptr_t) asc + sizeof(asc))
                        asc[sizeof(asc) - 1] = '\0';
                    else
                        *name8 = '\0';
                }
                qDebug() << "NTFS Volume Name:" << asc;
                tsk_fs_file_close(tmpfile);
                break;
            case TSK_FS_TYPE_EXFAT:
                fatfs = (FATFS_INFO*)tmpfsinfo;
                if((tmpfile = tsk_fs_file_alloc(tmpfsinfo)) == NULL)
                {
                    // log error here
                }
                if((tmpfile->meta = tsk_fs_meta_alloc(FATFS_FILE_CONTENT_LEN)) == NULL)
                {
                    // log error here
                }
                if((databuffer = (char*)tsk_malloc(fatfs->ssize)) == NULL)
                {
                    // log error here
                }
                cursector = fatfs->rootsect;
                endsector = fatfs->firstdatasect + (fatfs->clustcnt * fatfs->csize) - 1;
                while(cursector < endsector)
                {

                }
                bytesread = tsk_fs_read_block(tmpfsinfo, cursector, databuffer, fatfs->ssize);
                if(bytesread != fatfs->ssize)
                {
                    // log error here
                }
                isallocsec = fatfs_is_sectalloc(fatfs, cursector);
                if(isallocsec == -1)
                {
                    // log error here
                }
                curinum = FATFS_SECT_2_INODE(fatfs, cursector);
                for(i = 0; i < fatfs->ssize; i+= sizeof(FATFS_DENTRY))
                {
                    dentry = (FATFS_DENTRY*)&(databuffer[i]);
                    if(exfatfs_get_enum_from_type(dentry->data[0]) == EXFATFS_DIR_ENTRY_TYPE_VOLUME_LABEL)
                    {
                        if(exfatfs_dinode_copy(fatfs, curinum, dentry, isallocsec, tmpfile) == TSK_OK)
                        {
                            qDebug() << "EXFAT Volume Name: " << tmpfile->meta->name2->name;
                            break;
                        }
                        else
                        {
                            // log error here
                        }
                    }
                }
                tsk_fs_file_close(tmpfile);
                free(databuffer);
                break;
            case TSK_FS_TYPE_FAT12:
                fatfs = (FATFS_INFO*)tmpfsinfo;
                fatsb = (FATXXFS_SB*)fatfs->boot_sector_buffer;
                qDebug() << fatsb->a.f16.vol_lab;
                printf("Volume Label (Boot Sector): %c%c%c%c%c%c%c%c%c%c%c\n", fatsb->a.f16.vol_lab[0], fatsb->a.f16.vol_lab[1], fatsb->a.f16.vol_lab[2], fatsb->a.f16.vol_lab[3], fatsb->a.f16.vol_lab[4], fatsb->a.f16.vol_lab[5], fatsb->a.f16.vol_lab[6], fatsb->a.f16.vol_lab[7], fatsb->a.f16.vol_lab[8], fatsb->a.f16.vol_lab[9], fatsb->a.f16.vol_lab[10]);
                if((databuffer = (char*) tsk_malloc(tmpfsinfo->block_size)) == NULL)
                {
                    // log error here
                }
                cnt = tsk_fs_read_block(tmpfsinfo, fatfs->rootsect, databuffer, tmpfsinfo->block_size);
                if(cnt != tmpfsinfo->block_size)
                {
                    // log error here
                }
                tmpfatdentry = NULL;
                if(fatfs->ssize <= tmpfsinfo->block_size)
                {
                    curentry = (FATXXFS_DENTRY*)databuffer;
                    for(int i=0; i < fatfs->ssize; i += sizeof(*curentry))
                    {
                        if(curentry->attrib == FATFS_ATTR_VOLUME)
                        {
                            tmpfatdentry = curentry;
                            break;
                        }
                        curentry++;
                    }
                }
                qDebug() << "FAT12 Volume Label: " << tmpfatdentry->name;
                free(databuffer);
                break;
            case TSK_FS_TYPE_FAT16:
                fatfs = (FATFS_INFO*)tmpfsinfo;
                fatsb = (FATXXFS_SB*)fatfs->boot_sector_buffer;
                qDebug() << fatsb->a.f16.vol_lab;
                printf("Volume Label (Boot Sector): %c%c%c%c%c%c%c%c%c%c%c\n", fatsb->a.f16.vol_lab[0], fatsb->a.f16.vol_lab[1], fatsb->a.f16.vol_lab[2], fatsb->a.f16.vol_lab[3], fatsb->a.f16.vol_lab[4], fatsb->a.f16.vol_lab[5], fatsb->a.f16.vol_lab[6], fatsb->a.f16.vol_lab[7], fatsb->a.f16.vol_lab[8], fatsb->a.f16.vol_lab[9], fatsb->a.f16.vol_lab[10]);
                if((databuffer = (char*) tsk_malloc(tmpfsinfo->block_size)) == NULL)
                {
                    // log error here
                }
                cnt = tsk_fs_read_block(tmpfsinfo, fatfs->rootsect, databuffer, tmpfsinfo->block_size);
                if(cnt != tmpfsinfo->block_size)
                {
                    // log error here
                }
                tmpfatdentry = NULL;
                if(fatfs->ssize <= tmpfsinfo->block_size)
                {
                    curentry = (FATXXFS_DENTRY*)databuffer;
                    for(int i=0; i < fatfs->ssize; i += sizeof(*curentry))
                    {
                        if(curentry->attrib == FATFS_ATTR_VOLUME)
                        {
                            tmpfatdentry = curentry;
                            break;
                        }
                        curentry++;
                    }
                }
                qDebug() << "FAT16 Volume Label: " << tmpfatdentry->name;
                free(databuffer);
                break;
            case TSK_FS_TYPE_FAT32:
                fatfs = (FATFS_INFO*)tmpfsinfo;
                fatsb = (FATXXFS_SB*)fatfs->boot_sector_buffer;
                qDebug() << fatsb->a.f32.vol_lab;
                printf("Volume Label (Boot Sector): %c%c%c%c%c%c%c%c%c%c%c\n", fatsb->a.f32.vol_lab[0], fatsb->a.f32.vol_lab[1], fatsb->a.f32.vol_lab[2], fatsb->a.f32.vol_lab[3], fatsb->a.f32.vol_lab[4], fatsb->a.f32.vol_lab[5], fatsb->a.f32.vol_lab[6], fatsb->a.f32.vol_lab[7], fatsb->a.f32.vol_lab[8], fatsb->a.f32.vol_lab[9], fatsb->a.f32.vol_lab[10]);
                if((databuffer = (char*) tsk_malloc(tmpfsinfo->block_size)) == NULL)
                {
                    // log error here
                }
                cnt = tsk_fs_read_block(tmpfsinfo, fatfs->rootsect, databuffer, tmpfsinfo->block_size);
                if(cnt != tmpfsinfo->block_size)
                {
                    // log error here
                }
                tmpfatdentry = NULL;
                if(fatfs->ssize <= tmpfsinfo->block_size)
                {
                    curentry = (FATXXFS_DENTRY*)databuffer;
                    for(int i=0; i < fatfs->ssize; i += sizeof(*curentry))
                    {
                        if(curentry->attrib == FATFS_ATTR_VOLUME)
                        {
                            tmpfatdentry = curentry;
                            break;
                        }
                        curentry++;
                    }
                }
                qDebug() << "FAT32 Volume Label: " << tmpfatdentry->name;
                free(databuffer);
                break;
            case TSK_FS_TYPE_FFS1:
                qDebug() << "FFS1";
                break;
            case TSK_FS_TYPE_FFS1B:
                qDebug() << "FFS1B";
                break;
            case TSK_FS_TYPE_FFS2:
                ffs = (FFS_INFO*)tmpfsinfo;
                sb1 = ffs->fs.sb1;
                sb2 = ffs->fs.sb2;
                qDebug() << "FFS2 Volume label: " << sb2->volname;
                break;
            case TSK_FS_TYPE_EXT2:
                ext2fs = (EXT2FS_INFO*)tmpfsinfo;
                sb = ext2fs->fs;
                qDebug() << "EXT2 Volume name: " << sb->s_volume_name;
                break;
            case TSK_FS_TYPE_EXT3:
                ext2fs = (EXT2FS_INFO*)tmpfsinfo;
                sb = ext2fs->fs;
                qDebug() << "EXT3 Volume name: " << sb->s_volume_name;
                break;
            case TSK_FS_TYPE_EXT4:
                ext2fs = (EXT2FS_INFO*)tmpfsinfo;
                sb = ext2fs->fs;
                qDebug() << "EXT4 Volume name: " << sb->s_volume_name;
                break;
            case TSK_FS_TYPE_RAW:
                qDebug() << "no file system. store 0, \"\", or message for respective variables";
                break;
            case TSK_FS_TYPE_ISO9660:
                a = 0;
                iso = (ISO_INFO*)tmpfsinfo;
                p = iso->pvd;
                for(p = iso->pvd; p!= NULL; p = p->next)
                {
                    a++;
                    qDebug() << "ISO9660 vol name: " << p->pvd.vol_id;
                }
                a = 0;
                for(s = iso->svd; s!= NULL; s = s->next)
                {
                    a++;
                    qDebug() << "ISO9660 vol name: " << s->svd.vol_id;
                }
                qDebug() << "ISO9660";
                break;
            case TSK_FS_TYPE_HFS:
                hfs = (HFS_INFO*)tmpfsinfo;
                hsb = hfs->fs;
                memset((char*)&key, 0, sizeof(hfs_btree_key_cat));
                cnid_to_array((uint32_t)HFS_ROOT_INUM, key.parent_cnid);
                off = hfs_cat_get_record_offset(hfs, &key);
                if(off == 0)
                {
                    // log error here
                }
                if(hfs_cat_read_thread_record(hfs, off, &thread))
                {
                    // log error here
                }

                memset((char*)&key, 0, sizeof(hfs_btree_key_cat));
                memcpy((char*)key.parent_cnid, (char*)thread.parent_cnid, sizeof(key.parent_cnid));
                memcpy((char*)&key.name, (char*)&thread.name, sizeof(key.name));
                off = hfs_cat_get_record_offset(hfs, &key);
                if(off = 0)
                {
                    // log error here
                }
                if(hfs_cat_read_file_folder_record(hfs, off, &record))
                {
                    // log error here
                }
                //if(tsk_getU16(tmpfsinfo->endian, record.file.std.rec_type) == HFS_FOLDER_RECORD)
                memcpy((char*)&entry->thread, (char*)&thread, sizeof(hfs_thread));
                entry->flags = TSK_FS_META_FLAG_ALLOC | TSK_FS_META_FLAG_USED;
                entry->inum = HFS_ROOT_INUM;
                if(follow_hard_link)
                {
                    unsigned char is_err;
                    TSK_INUM_T target_cnid = hfs_follow_hard_link(hfs, &(entry->cat), &is_err);
                    if(is_err > 1)
                    {
                        // log error here
                    }
                    if(target_cnid != HFS_ROOT_INUM)
                    {
                        uint8_t res = hfs_cat_file_lookup(hfs, target_cnid, entry, FALSE);
                        if(res != 0)
                        {
                            // log error here
                        }
                    }
                //}
                // NEED TO EXPAND THE HFS_CAT_FILE_LOOKUP() FUNCTION AND THE 
                if(hfs_cat_file_lookup(hfs, HFS_ROOT_INUM, &hfsentry, FALSE))
                {
                    // log error here
                }
                if(hfs_UTF16toUTF8(tmpfsinfo, hfsentry.thread.name.unicode, tsk_getu16(tmpfsinfo->endian, hfsentry.thread.name.length), fn, HFS_MAXNAMLEN + 1, HFS_U16U8_FLAG_REPLACE_SLASH))
                {
                    // log error here.
                }
                 //continuance
                qDebug() << "HFS Volume Name: " << fn;
                free(fn);
                break;
            case TSK_FS_TYPE_YAFFS2:
                qDebug() << "YAFFS2 no volume name, might want other properties though";
                break;
            case TSK_FS_TYPE_SWAP:
                qDebug() << "no file system. store 0, \"\", or message for respective variables";
                break;
           default:
                qDebug() << "what to do for default???";
                break;
        }
    }

     *
     */
