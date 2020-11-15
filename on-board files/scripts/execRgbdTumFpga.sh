function execOnOneDataset(){
    result_dir="../results/PSPL/$1"
    output_filepath="${result_dir}/output.txt"

    mkdir ${result_dir}
    touch ${output_filepath}
    echo "executing with dataset $1, this may takes about 5 mins..."
    ../exec/rgbd_tum_fpga ../vocabulary/ORBvoc.txt ../dataset/TUM/parameters/TUM_ours.yaml ../dataset/TUM/$1/ ../dataset/TUM/associations/$1.txt ../results/PSPL/$1 > ${output_filepath}
    echo "done, output has been written to ${output_filepath}"
}

if [ $# -lt 1 ];
then
    echo "usage: ./execRgbdTum.sh <dataset1> <dataset2> ..."
    exit
fi

index=1
for arg in $*
do
    execOnOneDataset $arg
done