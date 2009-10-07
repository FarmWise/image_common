/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2009, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#include "image_transport/subscriber.h"
#include "image_transport/subscriber_plugin.h"
#include <pluginlib/class_loader.h>
#include <boost/scoped_ptr.hpp>

namespace image_transport {

struct Subscriber::Impl
{
  Impl()
    : loader("image_transport", "image_transport::SubscriberPlugin")
  {
  }
  
  pluginlib::ClassLoader<SubscriberPlugin> loader;
  boost::scoped_ptr<SubscriberPlugin> subscriber;
};

Subscriber::Subscriber()
  : impl_(new Impl)
{
}

Subscriber::Subscriber(const Subscriber& rhs)
  : impl_(rhs.impl_)
{
}

Subscriber::~Subscriber()
{
}

Subscriber::Subscriber(ros::NodeHandle& nh, const std::string& base_topic, uint32_t queue_size,
                       const boost::function<void(const sensor_msgs::ImageConstPtr&)>& callback,
                       const ros::VoidPtr& tracked_object, const TransportHints& transport_hints)
  : impl_(new Impl)
{
  std::string lookup_name = SubscriberPlugin::getLookupName(transport_hints.getTransport());
  impl_->subscriber.reset( impl_->loader.createClassInstance(lookup_name) );
  impl_->subscriber->subscribe(nh, base_topic, queue_size, callback, tracked_object, transport_hints);
}

std::string Subscriber::getTopic() const
{
  return impl_->subscriber->getTopic();
}

void Subscriber::shutdown()
{
  if (impl_->subscriber) {
    impl_->subscriber->shutdown();
    impl_->subscriber.reset();
  }
}

Subscriber::operator void*() const
{
  return (impl_ && impl_->subscriber) ? (void*)1 : (void*)0;
}

} //namespace image_transport
