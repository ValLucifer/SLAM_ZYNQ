function evalOneDataset(){
    result_dir="../results/PSPL/$1"
    output_filepath="${result_dir}/ate.txt"

    touch ${output_filepath}
    echo "evaluating ate for $1"
    python ../rgbd_benchmark_tools/scripts/evaluate_ate.py ../dataset/TUM/$1/groundtruth.txt $result_dir/CameraTrajectory.txt --verbose --plot plot.png > $output_filepath
    mv ./plot.png $result_dir
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
    evalOneDataset $arg
done