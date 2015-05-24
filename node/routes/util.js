var request = require('request');
var nconf = require('nconf');
var config;

exports.setConfig = function(conf) {
	config = conf;
};

exports.runPost = function(data, action, coreId, res) {
	console.log("action: " + action);
	console.log("coreId: " + coreId);
	console.log("data: " + data);

	if(coreId == "all")
	{
		var cores = config.get("cores");
		var resData = [];
		async.each(cores, function(core, callback){
			var url = buildUrl("run", core.id);
			request.post(url, function(err, response, body) {
				resData.push(body);
				callback();
				// res.send(body);
			}).form(this.buildFormData(data));
		}, function(err){
			if(err){
				console.log("Error making API call to core ", core);	
			} else {
				console.log("resData: ", resData);
				res.send(resData);
			}
		});
	}
	else
	{
		var url = this.buildUrl("run", coreId);
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(this.buildFormData(data));	
	}
	
};

exports.buildFormData = function(params) {
	var access_token = config.get("access_token");
	var formData = {"access_token":access_token ,"params":params};
	console.log("formData: ", formData);
	return formData;
};

exports.buildUrl = function(action, coreId) {
	var sparkUrl = config.get("spark_url");
	var url = sparkUrl + "/" + coreId + "/" + action;
	console.log("url: ", url);
	return url;
};

exports.buildParamUrl = function(coreId) {
	var sparkUrl = config.get("spark_url");
	var url = sparkUrl + "/" + coreId + "/parameters?access_token=" + config.get("access_token");
	console.log("paramUrl: ", url);
	return url;
};