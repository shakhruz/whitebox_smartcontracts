export KYLIN_TEST_ACCOUNT=whiteboxmain

export KYLIN_TEST_PUBLIC_KEY=curl http://faucet.cryptokylin.io/get_token?$KYLIN_TEST_ACCOUNT
curl http://faucet.cryptokylin.io/get_token?$KYLIN_TEST_ACCOUNT

cleos -u $EOS_ENDPOINT system delegatebw $KYLIN_TEST_ACCOUNT $KYLIN_TEST_ACCOUNT "50.0000 EOS" "100.0000 EOS" -p $KYLIN_TEST_ACCOUNT@owner

export KYLIN_TEST_PUBLIC_KEY=EOS6azSZ3XCqF1JoTPfE5KBoijgvr9zN1Jnt2ZTSYthqUPkZbPgMp
export EOS_ENDPOINT=https://kylin-dsp-2.liquidapps.io

cleos wallet create --file wallet_password.pwd
cleos wallet import --private-key 5JhNB7c8awzXRpxYU8FBWKh1EGTuP9TP8P651sarkVQb....

# select packages - vram and vaccounts
cleos -u $EOS_ENDPOINT push action dappservices selectpkg "{\"owner\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"ipfsservice1\",\"package\":\"package1\"}" -p $KYLIN_TEST_ACCOUNT@owner
cleos -u $EOS_ENDPOINT push action dappservices selectpkg "{\"owner\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"accountless1\",\"package\":\"package1\"}" -p $KYLIN_TEST_ACCOUNT@owner

# stake DAPP tokens
cleos -u $EOS_ENDPOINT push action dappservices stake "{\"from\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"ipfsservice1\",\"quantity\":\"10.0000 DAPP\"}" -p $KYLIN_TEST_ACCOUNT@owner
cleos -u $EOS_ENDPOINT push action dappservices stake "{\"from\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"accountless1\",\"quantity\":\"10.0000 DAPP\"}" -p $KYLIN_TEST_ACCOUNT@owner

# deploy contract
cleos -u $EOS_ENDPOINT set contract $KYLIN_TEST_ACCOUNT ./contracts/eos/whitebox -p $KYLIN_TEST_ACCOUNT@owner

# buy ram
cleos -u $EOS_ENDPOINT system buyram $KYLIN_TEST_ACCOUNT $KYLIN_TEST_ACCOUNT "50.0000 EOS" -p $KYLIN_TEST_ACCOUNT@owner

cleos -u $EOS_ENDPOINT set account permission $KYLIN_TEST_ACCOUNT active "{\"threshold\":1,\"keys\":[{\"weight\":1,\"key\":\"$KYLIN_TEST_PUBLIC_KEY\"}],\"accounts\":[{\"permission\":{\"actor\":\"$KYLIN_TEST_ACCOUNT\",\"permission\":\"eosio.code\"},\"weight\":1}]}" owner -p $KYLIN_TEST_ACCOUNT@owner

# stake resources

cleos -u $EOS_ENDPOINT system delegatebw $KYLIN_TEST_ACCOUNT $KYLIN_TEST_ACCOUNT "250.0 EOS" "200.0 EOS" -p $KYLIN_TEST_ACCOUNT@owner