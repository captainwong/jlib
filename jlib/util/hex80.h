/**
 * https://developer.arm.com/documentation/ka003292/latest
 *
 * The Intel HEX file is an ASCII text file with lines of text that follow the Intel HEX file format.
 * Each line in an Intel HEX file contains one HEX record.
 * These records are made up of hexadecimal numbers that represent machine language code and/or constant data.
 * Intel HEX files are often used to transfer the program and data that would be stored in a ROM or EPROM.
 * Most EPROM programmers or emulators can use Intel HEX files.
 *
 *
 * ## Record Format
 * An Intel HEX file is composed of any number of HEX records.
 * Each record is made up of five fields that are arranged in the following format:
 *
 * :llaaaatt[dd...]cc
 * Each group of letters corresponds to a different field, and each letter represents a single hexadecimal digit.
 * Each field is composed of at least two hexadecimal digits-which make up a byte-as described below:
 *
 * - : is the colon that starts every Intel HEX record.
 * - ll is the record-length field that represents the number of data bytes (dd) in the record.
 * - aaaa is the address field that represents the starting address for subsequent data in the record.
 * - tt is the field that represents the HEX record type, which may be one of the following:
 *     - 00 - data record
 *     - 01 - end-of-file record
 *     - 02 - extended segment address record
 *     - 04 - extended linear address record
 *     - 05 - start linear address record (MDK-ARM only)
 * - dd is a data field that represents one byte of data. A record may have multiple data bytes.
 *     The number of data bytes in the record must match the number specified by the ll field.
 * - cc is the checksum field that represents the checksum of the record.
 *     The checksum is calculated by summing the values of all hexadecimal digit pairs in the record modulo 256 and taking the two's complement.
 *
 *
 * ## 00 Data Records
 *
 * The Intel HEX file is made up of any number of data records that are terminated with a carriage return and a linefeed. Data records appear as follows:
 *
 * :10246200464C5549442050524F46494C4500464C33
 *
 * This record is decoded as follows:
 *
 * :10246200464C5549442050524F46494C4500464C33
 * |||||||||||                              CC->Checksum
 * |||||||||DD->Data
 * |||||||TT->Record Type
 * |||AAAA->Address
 * |LL->Record Length
 * :->Colon
 *
 * where:
 *
 * - 10 is the number of data bytes in the record.
 * - 2462 is the address where the data are to be located in memory.
 * - 00 is the record type 00 (a data record).
 * - 464C...464C is the data.
 * - 33 is the checksum of the record.
 *
 *
 * ## 04 Extended Linear Address Records (HEX386)
 * Extended linear address records are also known as 32-bit address records and HEX386 records.
 * These records contain the upper 16 bits (bits 16-31) of the data address.
 * The extended linear address record always has two data bytes and appears as follows:
 *
 * :02000004FFFFFC
 *
 * where:
 * - 02 is the number of data bytes in the record.
 * - 0000 is the address field. For the extended linear address record, this field is always 0000.
 * - 04 is the record type 04 (an extended linear address record).
 * - FFFF is the upper 16 bits of the address.
 * - FC is the checksum of the record and is calculated as 01h + NOT(02h + 00h + 00h + 04h + FFh + FFh).
 *
 * When an extended linear address record is read, the extended linear address stored in the data field is saved and is
 * applied to subsequent records read from the Intel HEX file.
 * The linear address remains effective until changed by another extended address record.
 *
 * The absolute-memory address of a data record is obtained by adding the address field in the record to the
 * shifted address data from the extended linear address record. The following example illustrates this process..
 *
 * Address from the data record's address field      2462
 * Extended linear address record data field     FFFF
 *                                               --------
 * Absolute-memory address                       FFFF2462
 *
 *
 * ## 05 Start Linear Address Records (MDK-ARM only)
 *
 * Start linear address records specify the start address of the application.
 * These records contain the full linear 32 bit address. The start linear address record always has four data bytes and appears as follows:
 *
 * :04000005000000CD2A
 *
 * where:
 * - 04 is the number of data bytes in the record.
 * - 0000 is the address field. For the start linear address record, this field is always 0000.
 * - 05 is the record type 05 (a start linear address record).
 * - 000000CD is the 4 byte linear start address of the application.
 * - 2A is the checksum of the record and is calculated as 01h + NOT(04h + 00h + 00h + 05h + 00h + 00h + 00h + CDh).
 *
 * The Start Linear Address specifies the address of the __main (pre-main) function but not the address of
 * the startup code which usually calls __main after calling SystemInit(). An odd linear start address
 * specifies that __main is compiled for the Thumb instruction set.
 *
 * The Start Linear Address Record can appear anywhere in hex file.
 * In most cases this record can be ignored because it does not contain information which is needed to program flash memory.
 *
 *
 * ## 01 End-of-File (EOF) Records
 *
 * An Intel HEX file must end with an end-of-file (EOF) record.
 * This record must have the value 01 in the record type field. An EOF record always appears as follows:
 *
 * :00000001FF
 *
 * where:
 *
 * - 00 is the number of data bytes in the record.
 * - 0000 is the address where the data are to be located in memory.
 *        The address in end-of-file records is meaningless and is ignored.
 *        An address of 0000h is typical.
 * - 01 is the record type 01 (an end-of-file record).
 * - FF is the checksum of the record and is calculated as 01h + NOT(00h + 00h + 00h + 01h).
 */

#ifndef __INTEL_HEX80_H__
#define __INTEL_HEX80_H__

#include <assert.h>
#include <ctype.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "hex.h"
#include "std_util.h"
#include "str_util.h"

namespace jlib {

#pragma pack(1)
typedef union {
    uint8_t raw[260];
    struct {
        uint8_t colon;
        uint8_t len;    // dat len
        uint32_t addr;  // warning: addr is converted to uint32_t to avoid overflow when adding extended address
        uint8_t type;
        uint8_t dat[1];
    } st;
} hex80_record_t;

typedef struct {
    uint32_t addr;
    std::vector<uint8_t> dat;
} hex80_code_snippet_t;
#pragma pack()

#define HEX80_RECORD_TYPE_DATA 0x00
#define HEX80_RECORD_TYPE_EOF 0x01
#define HEX80_RECORD_TYPE_EXTENDED_SEGMENT_ADDRESS 0x02
#define HEX80_RECORD_TYPE_EXTENDED_LINEAR_ADDRESS 0x04
#define HEX80_RECORD_TYPE_START_LINEAR_ADDRESS 0x05

// now we only support the data and eof type
#define is_supported_record_type(t) ((t) == HEX80_RECORD_TYPE_DATA ||                     \
                                     (t) == HEX80_RECORD_TYPE_EXTENDED_SEGMENT_ADDRESS || \
                                     (t) == HEX80_RECORD_TYPE_EXTENDED_LINEAR_ADDRESS ||  \
                                     (t) == HEX80_RECORD_TYPE_START_LINEAR_ADDRESS ||     \
                                     (t) == HEX80_RECORD_TYPE_EOF)

inline uint8_t calc_hex80_record_sum(const hex80_record_t& rec) {
    uint8_t sum = 0;
    // sum += rec.st.colon; // no colon for sum
    sum += rec.st.len;
    sum += (rec.st.addr & 0xFF);
    sum += (rec.st.addr >> 8) & 0xFF;
    sum += rec.st.type;
    for (uint8_t i = 0; i < rec.st.len; i++) {
        sum += rec.st.dat[i];
    }
    return -sum;
}

// validate and convert line (:llaaaatt[dd...]cc) to hex80 record
inline bool line2hex80record(std::string line, uint16_t address, hex80_record_t& record) {
    size_t len = line.size();
    if (len == 0 || line[0] != ':') {
        return false;
    }

    // :
    record.st.colon = line[0];
    const char* p = line.c_str();
    p++;
    len--;

    // ll
    if (!read_hex(p, len, record.st.len)) {
        return false;
    }
    p += 2;
    len -= 2;
    if (len < record.st.len + 2U + 1U + 1U) {  // 2 for addr, 1 for type, 1 for sum
        return false;
    }

    // aaaa
    uint16_t addr16 = 0;
    if (!read_hex(p, len, addr16)) {
        return false;
    }
    p += 4;
    len -= 4;

    // tt
    if (!read_hex(p, len, record.st.type)) {
        return false;
    }
    p += 2;
    len -= 2;
    if (!is_supported_record_type(record.st.type)) {
        return false;
    }

    // dd...
    for (size_t i = 0; i < record.st.len; i++) {
        if (!read_hex(p, len, record.st.dat[i])) {
            return false;
        }
        p += 2;
        len -= 2;
    }

    // cc
    uint8_t sum;
    if (!read_hex(p, len, sum)) {
        return false;
    }
    uint8_t mysum = calc_hex80_record_sum(record);
    if (mysum != sum) {
        return false;
    }

    record.st.addr = address + addr16;
    return true;
}

// convert intel hex-80 format to vector of hex80_record_t
// return 0 for success
inline int hex80_to_records(const std::string& hex80_content, std::vector<hex80_record_t>& records) {
    uint16_t address = 0;
    auto lines = jlib::split<std::string>(hex80_content, "\n");
    for (auto& line : lines) {
        hex80_record_t record{0};
        if (line2hex80record(line, address, record)) {
            records.push_back(record);
            if (record.st.type == HEX80_RECORD_TYPE_EXTENDED_LINEAR_ADDRESS && record.st.len == 2) {
                address = (record.st.dat[0] << 8) | record.st.dat[1];
                address <<= 16;
            } else if (record.st.type == HEX80_RECORD_TYPE_EXTENDED_SEGMENT_ADDRESS && record.st.len == 2) {
                address = (record.st.dat[0] << 8) | record.st.dat[1];
                address <<= 8;
            } else if (record.st.type == HEX80_RECORD_TYPE_START_LINEAR_ADDRESS && record.st.len == 4) {
                // we can ignore this type for now
            } else if (record.st.type == HEX80_RECORD_TYPE_EOF) {
                break;
            }
        }
    }
    return 0;
}

// merge hex80 records to snippets based on their address
inline void merge_hex80_records(const std::vector<hex80_record_t>& records, std::vector<hex80_code_snippet_t>& snippets) {
    for (const auto& rec : records) {
        if (!is_supported_record_type(rec.st.type)) {
            continue;
        }
        if (rec.st.type == HEX80_RECORD_TYPE_EOF) {
            break;
        }
        if (snippets.empty()) {
            hex80_code_snippet_t snippet;
            snippet.addr = rec.st.addr;
            snippet.dat.insert(snippet.dat.end(), rec.st.dat, rec.st.dat + rec.st.len);
            snippets.push_back(snippet);
        } else {
            auto& last = snippets.back();
            if (last.addr + last.dat.size() == rec.st.addr) {
                last.dat.insert(last.dat.end(), rec.st.dat, rec.st.dat + rec.st.len);
            } else {
                hex80_code_snippet_t snippet;
                snippet.addr = rec.st.addr;
                snippet.dat.insert(snippet.dat.end(), rec.st.dat, rec.st.dat + rec.st.len);
                snippets.push_back(snippet);
            }
        }
    }
}

// convert intel hex-80 format to binary
// return 0 for success
inline int hex80_to_bin(const std::string& hex80_content, uint8_t filler, std::vector<uint8_t>& bin_data) {
    std::vector<hex80_record_t> records;
    std::vector<hex80_code_snippet_t> snippets;
    if (hex80_to_records(hex80_content, records)) {
        return -1;
    }
    merge_hex80_records(records, snippets);
    for (const auto& snippet : snippets) {
        auto offset = snippet.addr;
        auto size = snippet.dat.size();
        if (bin_data.size() < offset) {
            // fill bin_data with filler until offset
            bin_data.resize(offset, filler);
        }
        std::copy(snippet.dat.begin(), snippet.dat.end(), bin_data.begin() + offset);
    }
    return 0;
}

// convert binary data to hex-80 string
// return 0 for success
inline int bin_to_hex80(const std::vector<uint8_t>& bin_data, uint8_t filler, std::string& hex80_content) {
    // TODO: implement this function
    return 0;
}

}  // namespace jlib

#endif /* __INTEL_HEX80_H__ */
