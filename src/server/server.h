#pragma once

namespace clc { namespace server {

bool is_running();
void start(bool foreground = false);
void stop();

}} // namespace clc::server
