#include "application_layer_messages.h"
#include "commandline_parser.h"
#include "json_creator.h"
#include "pcap_reader.h"
#include "protocol_decoder.h"
#include "stream_writer.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string_view>

static constexpr std::string_view usage = R"( 
Usage: ./<binary-name> --out_file=response.json --inpcap_file=infile.pcap --logfile_path=log.txt
inpcap_file is a compulsory argument. The default output file is response.json. The default logfile path is log.txt.
    )";

int main(int argc, char **argv) {

  auto [inpcap_file, outfile_path, logfile_path] =
      commandline::parse_commandline(argc, argv);
  if (inpcap_file.empty() || argc == 1) {
    std::cout << usage << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ofstream output_ofs(outfile_path, std::ios::out);
  writer::StreamWriter W(output_ofs.is_open() ? output_ofs : std::cout);
  simba::JsonCreator J(W);
  std::ofstream log_ofs(logfile_path, std::ios::out);
  simba::ProtocolDecoder PD(J, log_ofs);
  reader::PcapReader Reader(PD, log_ofs);
  Reader.read_packets(inpcap_file);
  output_ofs.close();
  log_ofs.close();

  return 0;
}
