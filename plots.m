clear;

load result_ctxpp;
load result_sc;

ctxpp_10_idx = find(result_ctxpp(:,1)==10);
ctxpp_10 = result_ctxpp(ctxpp_10_idx,2:3);

ctxpp_20_idx = find(result_ctxpp(:,1)==20);
ctxpp_20 = result_ctxpp(ctxpp_20_idx,2:3);

ctxpp_30_idx = find(result_ctxpp(:,1)==30);
ctxpp_30 = result_ctxpp(ctxpp_30_idx,2:3);

ctxpp_500_idx = find(result_ctxpp(:,1)==500);
ctxpp_500 = result_ctxpp(ctxpp_500_idx,2:3);

hold on;
plot(ctxpp_10(:,1),ctxpp_10(:,2));
plot(ctxpp_20(:,1),ctxpp_20(:,2));
plot(ctxpp_30(:,1),ctxpp_30(:,2));
plot(ctxpp_500(:,1),ctxpp_500(:,2));

sc_10_idx = find(result_sc(:,1)==10);
sc_10 = result_sc(sc_10_idx,2:3);

sc_20_idx = find(result_sc(:,1)==20);
sc_20 = result_sc(sc_20_idx,2:3);

sc_30_idx = find(result_sc(:,1)==30);
sc_30 = result_sc(sc_30_idx,2:3);

sc_500_idx = find(result_sc(:,1)==500);
sc_500 = result_sc(sc_500_idx,2:3);

plot(sc_10(:,1),sc_10(:,2)  ,'r');
plot(sc_20(:,1),sc_20(:,2)  ,'r');
plot(sc_30(:,1),sc_30(:,2)  ,'r');
plot(sc_500(:,1),sc_500(:,2),'r');

