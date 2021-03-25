#include "../../jlib/win32/jinfo.h"
#include "../../jlib/util/space.h"
#include <iostream>
#include <stdlib.h>

#ifdef _WIN64
#pragma comment(lib, "jinfo_x64.lib")
#else
#pragma comment(lib, "jinfo.lib")
#endif // _WIN64



using namespace std;
using namespace jlib::win32::info;

int main()
{

	cout << "cpu\n" 
		<< "architecture:\t" << cpu::architecture_to_string(cpu::architecture()) << endl
		<< "cpu_id:\t" << cpu::cpu_id() << endl
		<< "vendor:\t" << cpu::vendor() << endl
		<< "brand:\t" << cpu::brand() << endl;

	auto kernal_version = system::kernel_version();
	auto mem = system::memory();
	cout << "\nsystem\n"
		<< "os_name:\t" << system::os_name() << endl
		<< "computer_name:\t" << system::computer_name() << endl
		<< "is_64bit_windows:\t" << system::is_64bit_windows() << endl
		<< "kernel_version:\t" << kernal_version.major << "." << kernal_version.minor << "." << kernal_version.patch << "." << kernal_version.build_number << endl
		<< "memory:\t" << mem.physical_available << "/" << mem.physical_total << endl
		<< "display_version:\t" << system::display_version() << endl
		<< "os_version:\t" << system::os_version() << endl
		<< "product_id:\t" << system::product_id() << endl
		<< "uuid:\t" << system::uuid() << endl
		<< "system_info_string:\t" << system::system_info_string() << endl
		;

	cout << "\nuser\n";
	for (auto user : user::users()) {
		cout << user.name << " "
			<< user.full_name << " "
			<< user.privilege << endl;
	}

	cout << "\ndiskdrive\nindex\tpartitions\tdevice_id\tmodel\tserial\tsize\n";
	for (auto&& disk : diskdrive::disk_drives()) {
		cout << disk.index << "\t" 
			<< disk.partitions << "\t" 
			<< disk.device_id << "\t" 
			<< disk.model << "\t" 
			<< disk.serial << "\t" << 
			jlib::human_readable_byte_count(disk.size, jlib::PositionalNotation::Decimal) << endl;
	}
	cout << "bootable_disk_serial:\t" << diskdrive::bootable_disk_serial() << endl;

	cout << "\nlogicaldrive\ndevice_id\tdrive_type\tfree_space\tsize\tpercent\tvolume_name\n";
	for (auto&& drive : logicaldrive::logical_drives()) {
		cout << drive.device_id << "\t"
			<< logicaldrive::drive_type_to_string(drive.drive_type) << "\t"
			<< jlib::human_readable_byte_count(drive.free_space, jlib::PositionalNotation::Decimal) << "/"
			<< jlib::human_readable_byte_count(drive.size, jlib::PositionalNotation::Decimal) << "\t"
			<< drive.free_space * 100.0 / drive.size << "%\t"
			<< drive.volume_name << endl;
	}

	cout << "\ngpu\n";
	for (auto&& gpu : gpu::gpus()) {
		cout << gpu.name << " " << jlib::human_readable_byte_count(gpu.memsz) << endl;
	}

	cout << "\nbaseboard\nmanufacturer:\t" << baseboard::manufacturer() << endl
		<< "product:\t" << baseboard::product() << endl
		<< "serial:\t" << baseboard::serial() << endl
		<< "bios_serial:\t" << baseboard::bios_serial() << endl;

	std::system("pause");
}
