export ACCOUNT=whitebxtoken
export KYLIN_TEST_PUBLIC_KEY=EOS8DcmyuY2CvAKe4oY1RTSsJuFd53M24d7caH5bEzicepAP1PJor
export DSP_ENDPOINT=https://kylin-dsp-1.liquidapps.io
export EOS_ENDPOINT=https://kylin-dsp-2.liquidapps.io

cleos wallet unlock --password <./wallet_password.pwd

# cleos -u $EOS_ENDPOINT set account permission $ACCOUNT active "{\"threshold\":1,\"keys\":[{\"weight\":1,\"key\":\"$KYLIN_TEST_PUBLIC_KEY\"}],\"accounts\":[{\"permission\":{\"actor\":\"$ACCOUNT\",\"permission\":\"eosio.code\"},\"weight\":1}]}" owner -p $ACCOUNT@active

# deploy contract
cleos -u $EOS_ENDPOINT set contract $ACCOUNT ./contracts/eos/whitebxtoken -p $ACCOUNT@active
