/**
* Copyright (c) 2023 Florian Porrmann, René Griessl, Jerome Brenig, Sigrun May
* This software is distributed under the terms of the MIT license
* which is available at https://opensource.org/licenses/MIT
*/

#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <malloc.h>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#define XDMA_VERBOSE

#include "IP_Cores/AxiDMA.h"
#include "IP_Cores/HLSCore.h"
#include "xdmaAccess.h"
#include "time.h"

const uint64_t DDR_BASE_ADDR = 0x3C000000;
const uint64_t DDR_SIZE      = 0x1000000;

const uint64_t HLS_CORE_ADDR     = 0x40000000;
const uint64_t AXI_DMA_ADDR      = 0x40400000;
const uint64_t AXI_DMA_TEST_ADDR = 0x41E00000;

#if 1
#define XAUTO_SEARCH_CTRL_BUS_ADDR_ROWS_DATA           0x10
#define XAUTO_SEARCH_CTRL_BUS_ADDR_SIZE_OF_STREAM_DATA 0x18
#define XAUTO_SEARCH_CTRL_BUS_ADDR_PRECISION_DATA      0x20

// const std::size_t ROW_COUNT          = 30;
// const std::size_t SIZE_OF_STEAM_DATA = 50000;
// const std::size_t PRECISION          = 7;

const std::size_t PADDING = 50;

const std::string INPUT_FILE = "/data/storage/jbrenig/fpga_data/INPUT.bin";
const std::string OUTPUT_FILE = "/data/storage/jbrenig/fpga_data/OUTPUT.bin";

float intToFloat(int a)
{
	return *(float*)&a;
}

int parseArgv(char *str) {
	int result = 0;
	int i = 0;
	while (str[i] != '\0') {
		result *= 10;
		result += str[i++] - 48;
	}
	return result;
}

int main(int argc, char* argv[])
{
	
	std::size_t ROW_COUNT = parseArgv(argv[1]);
	std::size_t SIZE_OF_STEAM_DATA = parseArgv(argv[2]);
	std::size_t PRECISION = parseArgv(argv[3]);
	std::size_t SIZE_ARR = PADDING * SIZE_OF_STEAM_DATA;

	std::cout << "ZYNQ Test - v0.1" << std::endl;

	XDMABuffer<int32_t> inputData  = XDMABuffer<int32_t>(SIZE_ARR, 0);
	XDMABuffer<int32_t> outputData = XDMABuffer<int32_t>(SIZE_OF_STEAM_DATA, 0);

	std::cout << "Init Zynq ... " << std::flush;
	XDMAPtr zynq = XDMA::Create<PetaLinuxBackend>();
	std::cout << "Done" << std::endl;

	std::cout << "Init HLS Core ... " << std::flush;
	HLSCore autoSearch(zynq, HLS_CORE_ADDR, "Auto Search");
	std::cout << "Done" << std::endl;

	std::cout << "Adding Memory Regions ... " << std::flush;
	zynq->AddMemoryRegion(XDMA::MemoryType::DDR, DDR_BASE_ADDR, DDR_SIZE);
	std::cout << "Done" << std::endl;

	std::cout << "Allocating Memory ... " << std::flush;
	Memory inputBuffer  = zynq->AllocMemoryDDR(inputData.size(), (uint64_t)(sizeof(uint32_t)));
	Memory outputBuffer = zynq->AllocMemoryDDR(outputData.size(), (uint64_t)(sizeof(uint32_t)));
	std::cout << "Done" << std::endl;

	std::cout << "Reading the input data from the file: " << INPUT_FILE << " ..." << std::flush;
	std::ifstream inputFile(INPUT_FILE, std::ios::binary);
	if(!inputFile.is_open())
	{
		std::cerr << "ERROR: Could not open the file: " << INPUT_FILE << std::endl;
		return -1;
	}

	char* buf = reinterpret_cast<char*>(inputData.data());

	for (std::size_t i = 0; i < SIZE_OF_STEAM_DATA; i++)
	{
		inputFile.read(buf, ROW_COUNT * sizeof(int32_t));
		buf += PADDING * sizeof(int32_t);
	}

	inputFile.close();
	std::cout << "Done" << std::endl;

	std::cout << "Setting up AxiDMA ... " << std::flush;
	AxiDMA<int32_t> axiDMA(zynq, AXI_DMA_ADDR);
	std::cout << "Done" << std::endl;
	// Trigger a reset in the AxiDMA
	axiDMA.Reset();

	std::cout << "Input Buffer : " << inputBuffer << std::endl;
	std::cout << "Output Buffer: " << outputBuffer << std::endl;

	double time1=0.0, tstart;
	try
	{
		autoSearch.SetDataAddr<uint32_t>(XAUTO_SEARCH_CTRL_BUS_ADDR_ROWS_DATA, ROW_COUNT);
		autoSearch.SetDataAddr<uint32_t>(XAUTO_SEARCH_CTRL_BUS_ADDR_SIZE_OF_STREAM_DATA, SIZE_OF_STEAM_DATA);
		autoSearch.SetDataAddr<uint32_t>(XAUTO_SEARCH_CTRL_BUS_ADDR_PRECISION_DATA, PRECISION);

		zynq->Write(inputBuffer, inputData.data());

		axiDMA.Start(inputBuffer, outputBuffer);

		tstart = clock();

		autoSearch.Start();
		autoSearch.WaitForFinish();
		time1 += clock() - tstart;
		time1 = time1/CLOCKS_PER_SEC;

		std::cout << "Waiting for the AxiDMA to finish ... " << std::endl;

		// Wait until the MM2S channel finishes (an interrupt occures on complete)
		if (axiDMA.WaitForFinish(DMAChannel::MM2S))
			std::cout << "Channel: MM2S finished successfully" << std::endl;

		// Wait until the S2MM channel finishes (an interrupt occures on complete)
		if (axiDMA.WaitForFinish(DMAChannel::S2MM))
			std::cout << "Channel: S2MM finished successfully" << std::endl;

		zynq->Read(outputBuffer, outputData.data());

		// Stop the AxiDMA engine
		axiDMA.Stop();

		std::cout << "Time elapsed in seconds: " << time1 << std::endl;

		std::cout << "Writing the output data to the file: output.bin ... " << std::flush;

		// Write the output data to a binary file
		std::ofstream outputFile(OUTPUT_FILE, std::ios::binary);
		outputFile.write((char*)outputData.data(), outputData.size() * sizeof(uint32_t));
		outputFile.close();

		std::cout << "Done" << std::endl;
	}
	catch (XDMAException& e)
	{
		std::cerr << "ERROR: " << std::endl
				  << e.what() << std::endl;
		return -1;
	}

	


	return EXIT_SUCCESS;
}
#elif 1

#define IN_DATA_ADDR  0x10
#define OUT_DATA_ADDR 0x18
#define ELEM_COUNT    0x20

#define BRAM_ADDR 0x40100000
#define BRAM_SIZE 0x00100000

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <data_count>" << std::endl;
		return -1;
	}

	std::size_t TEST_DATA_SIZE = std::stoul(argv[1]);

	std::cout << "ZYNQ Test MM - v0.1" << std::endl;

	XDMABuffer<uint32_t> inputData  = XDMABuffer<uint32_t>(TEST_DATA_SIZE, 0);
	XDMABuffer<uint32_t> outputData = XDMABuffer<uint32_t>(TEST_DATA_SIZE, 0);

	std::iota(inputData.begin(), inputData.end(), 0);

	std::cout << "Init XDMA ... " << std::flush;
	XDMAPtr zynq = XDMA::Create<PetaLinuxBackend>();
	std::cout << "Done" << std::endl;

	std::cout << "Init HLS Core ... " << std::flush;
	HLSCore autoSearch(zynq, HLS_CORE_ADDR, "Auto Search");
	std::cout << "Done" << std::endl;

	std::cout << "Adding Memory Regions ... " << std::flush;
	zynq->AddMemoryRegion(XDMA::MemoryType::DDR, DDR_BASE_ADDR, DDR_SIZE);
	zynq->AddMemoryRegion(XDMA::MemoryType::BRAM, BRAM_ADDR, BRAM_SIZE);
	std::cout << "Done" << std::endl;

	std::cout << "Allocating Memory ... " << std::flush;
	Memory inputBuffer  = zynq->AllocMemoryDDR(inputData.size(), (uint64_t)(sizeof(uint32_t)));
	Memory outputBuffer = zynq->AllocMemoryDDR(outputData.size(), (uint64_t)(sizeof(uint32_t)));
	std::cout << "Done" << std::endl;

	std::cout << "Input Buffer : " << inputBuffer << std::endl;
	std::cout << "Output Buffer: " << outputBuffer << std::endl;

	try
	{
		autoSearch.SetDataAddr<uint32_t>(IN_DATA_ADDR, inputBuffer.GetBaseAddr());
		autoSearch.SetDataAddr<uint32_t>(OUT_DATA_ADDR, outputBuffer.GetBaseAddr());
		autoSearch.SetDataAddr<uint32_t>(ELEM_COUNT, TEST_DATA_SIZE);

		zynq->Write(inputBuffer, inputData.data());

		autoSearch.Start();
		autoSearch.WaitForFinish();

		zynq->Read(outputBuffer, outputData.data());

		std::cout << "Writing the output data to the file: output.bin ... " << std::flush;

		// Write the output data to a binary file
		std::ofstream outputFile("output.bin", std::ios::binary);
		outputFile.write((char*)outputData.data(), outputData.size() * sizeof(uint32_t));
		outputFile.close();
		std::cout << "Done" << std::endl;

		// Compare the input and output data
		if (std::equal(inputData.begin(), inputData.end(), outputData.begin(), [](const uint32_t& a, const uint32_t& b) { return (a + 1) == b; }))
			std::cout << "Data is equal" << std::endl;
		else
			std::cout << "Data is not equal" << std::endl;
	}
	catch (XDMAException& e)
	{
		std::cerr << "ERROR: " << std::endl
				  << e.what() << std::endl;
		return -1;
	}

	return EXIT_SUCCESS;
}
#else

#define ELEM_COUNT 0x10

// const std::size_t TEST_DATA_SIZE = 49152;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <data_count>" << std::endl;
		return -1;
	}

	std::size_t TEST_DATA_SIZE = std::stoul(argv[1]);

	std::cout << "ZYNQ Test - v0.1" << std::endl;

	XDMABuffer<uint32_t> inputData  = XDMABuffer<uint32_t>(TEST_DATA_SIZE, 0);
	XDMABuffer<uint32_t> outputData = XDMABuffer<uint32_t>(TEST_DATA_SIZE, 0);

	std::iota(inputData.begin(), inputData.end(), 0);

	std::cout << "Init XDMA ... " << std::flush;
	XDMAPtr zynq = XDMA::Create<PetaLinuxBackend>();
	std::cout << "Done" << std::endl;

	std::cout << "Init HLS Core ... " << std::flush;
	HLSCore autoSearch(zynq, HLS_CORE_ADDR, "Auto Search");
	std::cout << "Done" << std::endl;

	std::cout << "Adding Memory Regions ... " << std::flush;
	zynq->AddMemoryRegion(XDMA::MemoryType::DDR, DDR_BASE_ADDR, DDR_SIZE);
	std::cout << "Done" << std::endl;

	std::cout << "Allocating Memory ... " << std::flush;
	Memory inputBuffer  = zynq->AllocMemoryDDR(inputData.size(), (uint64_t)(sizeof(uint32_t)));
	Memory outputBuffer = zynq->AllocMemoryDDR(outputData.size(), (uint64_t)(sizeof(uint32_t)));
	std::cout << "Done" << std::endl;

	std::cout << "Setting up AxiDMA ... " << std::flush;
	AxiDMA<uint32_t> axiDMA(zynq, AXI_DMA_TEST_ADDR);
	std::cout << "Done" << std::endl;
	// Trigger a reset in the AxiDMA
	axiDMA.Reset();

	std::cout << "Input Buffer : " << inputBuffer << std::endl;
	std::cout << "Output Buffer: " << outputBuffer << std::endl;

	try
	{
		autoSearch.SetDataAddr<uint32_t>(ELEM_COUNT, TEST_DATA_SIZE);

		zynq->Write(inputBuffer, inputData.data());

		// zynq::Logging::SetVerbosity(zynq::Logging::Verbosity::VB_DEBUG);

		axiDMA.Start(inputBuffer, outputBuffer);

		// zynq::Logging::SetVerbosity(zynq::Logging::Verbosity::VB_INFO);

		autoSearch.Start();
		autoSearch.WaitForFinish();

		std::cout << "Waiting for the AxiDMA to finish ... " << std::endl;

		// Wait until the MM2S channel finishes (an interrupt occures on complete)
		if (axiDMA.WaitForFinish(DMAChannel::MM2S))
			std::cout << "Channel: MM2S finished successfully" << std::endl;

		// Wait until the S2MM channel finishes (an interrupt occures on complete)
		if (axiDMA.WaitForFinish(DMAChannel::S2MM))
			std::cout << "Channel: S2MM finished successfully" << std::endl;

		zynq->Read(outputBuffer, outputData.data());

		// Stop the AxiDMA engine
		axiDMA.Stop();

		std::cout << "Writing the output data to the file: output.bin ... " << std::flush;

		// Write the output data to a binary file
		std::ofstream outputFile("output.bin", std::ios::binary);
		outputFile.write((char*)outputData.data(), outputData.size() * sizeof(uint32_t));
		outputFile.close();
		std::cout << "Done" << std::endl;

		// Compare the input and output data
		if (std::equal(inputData.begin(), inputData.end(), outputData.begin(), [](const uint32_t& a, const uint32_t& b) { return (a + 1) == b; }))
			std::cout << "Data is equal" << std::endl;
		else
			std::cout << "Data is not equal" << std::endl;
	}
	catch (XDMAException& e)
	{
		std::cerr << "ERROR: " << std::endl
				  << e.what() << std::endl;
		return -1;
	}

	return EXIT_SUCCESS;
}
#endif