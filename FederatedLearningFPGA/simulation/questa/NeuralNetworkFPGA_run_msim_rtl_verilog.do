transcript on
if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

vlog -vlog01compat -work work +incdir+C:/Development/FederatedEdgeComputing/FederatedLearningFPGA {C:/Development/FederatedEdgeComputing/FederatedLearningFPGA/NeuralNetworkFPGA.v}
vlog -vlog01compat -work work +incdir+C:/Development/FederatedEdgeComputing/FederatedLearningFPGA/output_files {C:/Development/FederatedEdgeComputing/FederatedLearningFPGA/output_files/Processor.v}

