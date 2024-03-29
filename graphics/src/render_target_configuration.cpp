#include <ovis/utils/log.hpp>
#include <ovis/utils/range.hpp>
#include <ovis/graphics/graphics_context.hpp>
#include <ovis/graphics/render_target.hpp>
#include <ovis/graphics/render_target_configuration.hpp>

namespace ovis {

RenderTargetConfiguration::RenderTargetConfiguration(GraphicsContext* context,
                                                     const RenderTargetConfigurationDescription& description)
    : GraphicsResource(context, Type::RENDER_TARGET_CONFIGURATION) {
  glGenFramebuffers(1, &m_frame_buffer);

  Bind();
  draw_buffers_.reserve(description.color_attachments.size());
  for (auto color_attachment : IndexRange(description.color_attachments)) {
    if (color_attachment.value() != nullptr) {
      color_attachment.value()->Attach(GL_COLOR_ATTACHMENT0 + color_attachment.index());
      width_ = color_attachment.value()->GetWidth();
      height_ = color_attachment.value()->GetHeight();
      assert(width_ > 0);
      assert(height_ > 0);
      draw_buffers_.push_back(GL_COLOR_ATTACHMENT0 + color_attachment.index());
    } else {
      draw_buffers_.push_back(GL_NONE);
    }
  }
  if (description.depth_attachment != nullptr) {
    description.depth_attachment->Attach(GL_DEPTH_ATTACHMENT);
  }
  if (description.stencil_attachment != nullptr) {
    description.stencil_attachment->Attach(GL_STENCIL_ATTACHMENT);
  }

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  assert(status == GL_FRAMEBUFFER_COMPLETE);
}

RenderTargetConfiguration::~RenderTargetConfiguration() {
  if (m_frame_buffer != 0) {
    if (context()->m_bound_frame_buffer == m_frame_buffer) {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      context()->m_bound_frame_buffer = 0;
    }
    glDeleteFramebuffers(1, &m_frame_buffer);
  }
}

void RenderTargetConfiguration::ClearColor(size_t color_attachment_index, const float clear_color[4]) {
  if (context()->scissoring_enabled_) {
    glDisable(GL_SCISSOR_TEST);
    context()->scissoring_enabled_ = false;
  }
  Bind();
#if OVIS_EMSCRIPTEN
  assert(color_attachment_index == 0);
  glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
  glClear(GL_COLOR_BUFFER_BIT);
#else
  glClearBufferfv(GL_COLOR, color_attachment_index, clear_color);
#endif
}

void RenderTargetConfiguration::ClearDepth(float depth) {
  if (context()->scissoring_enabled_) {
    glDisable(GL_SCISSOR_TEST);
    context()->scissoring_enabled_ = false;
  }
  if (!context()->depth_buffer_state_.write_enabled) {
    glDepthMask(true);
    context()->depth_buffer_state_.write_enabled = true;
  }
  Bind();
#if OVIS_EMSCRIPTEN
  glClearDepthf(depth);
  glClear(GL_DEPTH_BUFFER_BIT);
#else
  glClearBufferfv(GL_DEPTH, 0, &depth);
#endif
}

RenderTargetConfiguration::RenderTargetConfiguration(GraphicsContext* context, std::size_t width, std::size_t height)
    : GraphicsResource(context, Type::RENDER_TARGET_CONFIGURATION),
      m_frame_buffer(0),
      width_(width),
      height_(height),
      draw_buffers_({GL_COLOR_ATTACHMENT0}) {}

void RenderTargetConfiguration::Bind() {
  if (context()->m_bound_frame_buffer != m_frame_buffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
    context()->m_bound_frame_buffer = m_frame_buffer;
#if !OVIS_EMSCRIPTEN
    glDrawBuffers(draw_buffers_.size(), draw_buffers_.data());
#endif
  }
}

}  // namespace ovis
