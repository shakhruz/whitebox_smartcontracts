export ACCOUNT=whitebxtoken
export DSP_ENDPOINT=https://kylin-dsp-1.liquidapps.io
export EOS_ENDPOINT=https://kylin-dsp-2.liquidapps.io

cleos wallet unlock --password <./wallet_password.pwd

# deploy contract
cleos -u $EOS_ENDPOINT set contract $ACCOUNT ./contracts/eos/coldtoken -p $ACCOUNT@active
