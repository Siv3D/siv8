//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/DriveInfo.hpp>
# include <Siv3D/FormatLiteral.hpp>
# import <Foundation/Foundation.h>
# import <DiskArbitration/DiskArbitration.h>
# import <IOKit/IOKitLib.h>
# import <IOKit/storage/IOStorageDeviceCharacteristics.h>
# include <sys/param.h>
# include <sys/mount.h>
# include <unistd.h>

namespace s3d
{
	namespace System
	{
		Array<DriveInfo> EnumerateDrives()
		{
			Array<DriveInfo> driveInfos;

			@autoreleasepool
			{
				DASessionRef session = DASessionCreate(kCFAllocatorDefault);

				NSArray<NSURLResourceKey> *keys = @[
					NSURLVolumeLocalizedNameKey,
					NSURLVolumeTotalCapacityKey,
					NSURLVolumeAvailableCapacityKey,
					NSURLVolumeAvailableCapacityForImportantUsageKey,
					NSURLVolumeIsReadOnlyKey,
					NSURLVolumeIsRootFileSystemKey
				];

				NSVolumeEnumerationOptions options = NSVolumeEnumerationSkipHiddenVolumes;
				NSArray<NSURL *> *urls = [[NSFileManager defaultManager] mountedVolumeURLsIncludingResourceValuesForKeys:keys options:options];

				for (NSURL *url in urls)
				{
					struct statfs statInfo;
					if (statfs(url.fileSystemRepresentation, &statInfo) != 0)
					{
						continue;
					}

					const bool isNetwork = ((statInfo.f_flags & MNT_LOCAL) == 0);
					const bool isDev = (strncmp(statInfo.f_mntfromname, "/dev/", 5) == 0);
					if (not isNetwork && not isDev)
					{
						continue;
					}

					String rootPath = Unicode::FromUTF8(url.path.UTF8String);
					if (not rootPath.ends_with(U'/'))
					{
						rootPath += U'/';
					}

					String devicePath = Unicode::FromUTF8(statInfo.f_mntfromname);
					String fileSystem = Unicode::FromUTF8(statInfo.f_fstypename);
					String volumeName;
					String volumeID;
					DriveType driveType = DriveType::Unknown;
					uint64 availableSpaceBytes = 0;
					uint64 freeSpaceBytes = 0;
					uint64 totalSizeBytes = 0;
					bool isReadOnly = false;
					bool isCaseSensitive = false;
					bool isSystem = false;
					Optional<bool> isSSD = none;

					NSString *name = nil;
					if ([url getResourceValue:&name forKey:NSURLVolumeLocalizedNameKey error:nil] && name)
					{
						volumeName = Unicode::FromUTF8(name.UTF8String);
					}

					NSNumber *totalSize = nil;
					if ([url getResourceValue:&totalSize forKey:NSURLVolumeTotalCapacityKey error:nil] && totalSize)
					{
						totalSizeBytes = totalSize.unsignedLongLongValue;
					}

					NSNumber *availableSize = nil;
					if ([url getResourceValue:&availableSize forKey:NSURLVolumeAvailableCapacityForImportantUsageKey error:nil] && availableSize)
					{
						availableSpaceBytes = availableSize.unsignedLongLongValue;
					}
					else if ([url getResourceValue:&availableSize forKey:NSURLVolumeAvailableCapacityKey error:nil] && availableSize)
					{
						availableSpaceBytes = availableSize.unsignedLongLongValue;
					}

					NSNumber *freeSize = nil;
					if ([url getResourceValue:&freeSize forKey:NSURLVolumeAvailableCapacityKey error:nil] && freeSize)
					{
						freeSpaceBytes = freeSize.unsignedLongLongValue;
					}

					NSNumber *readOnlyFlag = nil;
					if ([url getResourceValue:&readOnlyFlag forKey:NSURLVolumeIsReadOnlyKey error:nil] && readOnlyFlag)
					{
						isReadOnly = readOnlyFlag.boolValue;
					}

					NSNumber *systemFlag = nil;
					if ([url getResourceValue:&systemFlag forKey:NSURLVolumeIsRootFileSystemKey error:nil] && systemFlag)
					{
						isSystem = systemFlag.boolValue;
					}

					const long caseSensitive = pathconf(url.fileSystemRepresentation, _PC_CASE_SENSITIVE);
					isCaseSensitive = (caseSensitive == 1);

					if (isNetwork)
					{
						driveType = DriveType::Network;
					}
					else if (strcmp(statInfo.f_fstypename, "cd9660") == 0 || strcmp(statInfo.f_fstypename, "udf") == 0)
					{
						driveType = DriveType::CDROM;
					}

					if (session)
					{
						DADiskRef disk = DADiskCreateFromVolumePath(kCFAllocatorDefault, session, (__bridge CFURLRef)url);
						if (disk)
						{
							CFDictionaryRef desc = DADiskCopyDescription(disk);
							if (desc)
							{
								CFUUIDRef uuidRef = (CFUUIDRef)CFDictionaryGetValue(desc, kDADiskDescriptionVolumeUUIDKey);
								if (uuidRef)
								{
									CFStringRef uuidStr = CFUUIDCreateString(kCFAllocatorDefault, uuidRef);
									if (uuidStr)
									{
										volumeID = Unicode::FromUTF8([(NSString *)(__bridge NSString *)uuidStr UTF8String]);
										CFRelease(uuidStr);
									}
								}

								if (driveType == DriveType::Unknown)
								{
									CFStringRef protocol = (CFStringRef)CFDictionaryGetValue(desc, kDADiskDescriptionDeviceProtocolKey);
									if (protocol)
									{
										if (CFStringCompare(protocol, CFSTR("Disk Image"), 0) == kCFCompareEqualTo ||
											CFStringCompare(protocol, CFSTR("Virtual Interface"), 0) == kCFCompareEqualTo)
										{
											driveType = DriveType::Virtual;
										}
									}

									CFStringRef model = (CFStringRef)CFDictionaryGetValue(desc, kDADiskDescriptionDeviceModelKey);
									if (driveType == DriveType::Unknown && model && CFStringFind(model, CFSTR("RAM"), kCFCompareCaseInsensitive).location != kCFNotFound)
									{
										driveType = DriveType::RAMDisk;
									}

									if (driveType == DriveType::Unknown)
									{
										CFBooleanRef removable = (CFBooleanRef)CFDictionaryGetValue(desc, kDADiskDescriptionMediaRemovableKey);
										if (removable && CFBooleanGetValue(removable))
										{
											driveType = DriveType::Removable;
										}
									}

									if (driveType == DriveType::Unknown)
									{
										CFBooleanRef internal = (CFBooleanRef)CFDictionaryGetValue(desc, kDADiskDescriptionDeviceInternalKey);
										if (internal && CFBooleanGetValue(internal))
										{
											driveType = DriveType::Fixed;
										}
									}
								}

								CFRelease(desc);
							}

							io_service_t media = DADiskCopyIOMedia(disk);
							if (media)
							{
								// ボリューム階層から上位の物理ストレージ（IOBlockStorageDevice等）まで遡りながら Device Characteristics を検索
								CFDictionaryRef characteristics = (CFDictionaryRef)IORegistryEntrySearchCFProperty(
									media,
									kIOServicePlane,
									CFSTR(kIOPropertyDeviceCharacteristicsKey), // "Device Characteristics"
									kCFAllocatorDefault,
									kIORegistryIterateRecursively | kIORegistryIterateParents);

								if (characteristics && CFGetTypeID(characteristics) == CFDictionaryGetTypeID())
								{
									CFStringRef mediumType = (CFStringRef)CFDictionaryGetValue(characteristics, CFSTR(kIOPropertyMediumTypeKey)); // "Medium Type"
									if (mediumType && CFGetTypeID(mediumType) == CFStringGetTypeID())
									{
										// "Solid State" であるかを判定
										isSSD = (CFStringCompare(mediumType, CFSTR(kIOPropertyMediumTypeSolidStateKey), 0) == kCFCompareEqualTo);
									}
									CFRelease(characteristics);
								}
								IOObjectRelease(media);
							}

							CFRelease(disk);
						}
					}

					if ((driveType == DriveType::Unknown) && isDev)
					{
						driveType = DriveType::Fixed;
					}

					driveInfos.push_back(DriveInfo{
						.rootPath = std::move(rootPath),
						.devicePath = std::move(devicePath),
						.volumeName = std::move(volumeName),
						.volumeID = std::move(volumeID),
						.fileSystem = std::move(fileSystem),
						.driveType = driveType,
						.availableSpaceBytes = availableSpaceBytes,
						.freeSpaceBytes = freeSpaceBytes,
						.totalSizeBytes = totalSizeBytes,
						.isReadOnly = isReadOnly,
						.isCaseSensitive = isCaseSensitive,
						.isSystem = isSystem,
						.isSSD = isSSD
					});
				}

				if (session)
				{
					CFRelease(session);
				}
			}

			return driveInfos;
		}
	}
}
