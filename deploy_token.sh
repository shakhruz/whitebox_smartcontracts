export ACCOUNT=meoswalletxx
export KYLIN_TEST_PUBLIC_KEY=EOS7xukfBzZVng3LcicwT8JqEtvpXdDxk9vKHpG3ycX51UaAZzAYD
export DSP_ENDPOINT=https://kylin-dsp-1.liquidapps.io
export EOS_ENDPOINT=https://kylin-dsp-2.liquidapps.io

cleos wallet unlock --password <./wallet_password.pwd

# cleos -u $EOS_ENDPOINT set account permission $ACCOUNT active "{\"threshold\":1,\"keys\":[{\"weight\":1,\"key\":\"$KYLIN_TEST_PUBLIC_KEY\"}],\"accounts\":[{\"permission\":{\"actor\":\"$ACCOUNT\",\"permission\":\"eosio.code\"},\"weight\":1}]}" owner -p $ACCOUNT@active
# export CHAIN_ID=5fff1dae8dc8e2fc4d5b23b2c7665c97f9e9d8edf2b6485a86ba311c25639191
# cleos -u $DSP_ENDPOINT push action $ACCOUNT xvinit "[\"$CHAIN_ID\"]" -p $ACCOUNT

# deploy contract
cleos -u $EOS_ENDPOINT set contract $ACCOUNT ./contracts/eos/meostoken -p $ACCOUNT@active

